// JetUncertainty
// v1.0
// Author: Yi Chen
// 
// This class gives you jet uncertainties
//

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "TF1.h"
#include "TF2.h"
#include "TF3.h"

class JetUncertainty
{
private:
   enum Type { TypeNone, TypeJetPT, TypeJetEta, TypeJetPhi, TypeJetArea, TypeRho };
   bool Initialized;
   double JetPT, JetEta, JetPhi, JetArea, Rho;
   std::vector<std::vector<Type>> BinTypes;
   std::vector<std::vector<double>> BinRanges;
   std::vector<std::vector<double>> PTBins;
   std::vector<std::vector<double>> ErrorLow;
   std::vector<std::vector<double>> ErrorHigh;
public:
   JetUncertainty()                  { Initialized = false; }
   JetUncertainty(std::string File)  { Initialized = false; Initialize(File); }
   ~JetUncertainty()                 {}
   void SetJetPT(double value)     { JetPT = value; }
   void SetJetEta(double value)    { JetEta = value; }
   void SetJetPhi(double value)    { JetPhi = value; }
   void SetJetArea(double value)   { JetArea = value; }
   void SetRho(double value)       { Rho = value; }
   void Initialize(std::string FileName);
   std::vector<std::string> BreakIntoParts(std::string Line);
   bool CheckDefinition(std::string Line);
   std::string StripBracket(std::string Line);
   JetUncertainty::Type ToType(std::string Line);
   std::pair<double, double> GetUncertainty();
   double GetValue(Type T);
};

void JetUncertainty::Initialize(std::string FileName)
{
   int nvar = 0, npar = 0;
   std::string CurrentFormula = "";
   std::vector<Type> CurrentDependencies;
   std::vector<Type> CurrentBinTypes;

   std::ifstream in(FileName.c_str());

   while(in)
   {
      char ch[1048576];
      ch[0] = '\0';
      in.getline(ch, 1048577, '\n');

      if(ch[0] == '\0')
         continue;

      bool IsDefinition = CheckDefinition(ch);
      std::vector<std::string> Parts = BreakIntoParts(StripBracket(ch));

      if(Parts.size() == 0)
         continue;

      if(IsDefinition == true)
      {
         // Found a definition line - update current formula

         nvar = atoi(Parts[0].c_str());
         if(Parts.size() <= nvar + 1)
            continue;
         npar = atoi(Parts[nvar+1].c_str());
         if(Parts.size() <= nvar + 1 + npar + 1)
            continue;

         CurrentFormula = Parts[nvar+1+npar+1];
         
         CurrentBinTypes.clear();
         for(int i = 0; i < nvar; i++)
            CurrentBinTypes.push_back(ToType(Parts[1+i]));

         CurrentDependencies.clear();
         for(int i = 0; i < npar; i++)
            CurrentDependencies.push_back(ToType(Parts[nvar+2+i]));
      }
      else
      {
         // Otherwise it's a line with actual JECs, add it to the list

         if(Parts.size() < nvar * 2 + 1)
            continue;

         BinTypes.push_back(CurrentBinTypes);

         std::vector<double> Ranges;
         for(int i = 0; i < nvar * 2; i++)
            Ranges.push_back(atof(Parts[i].c_str()));
         for(int i = 0; i + 1 < (int)Ranges.size(); i = i + 2)
            if(Ranges[i] > Ranges[i+1])
               std::swap(Ranges[i], Ranges[i+1]);
         BinRanges.push_back(Ranges);

         int N = atoi(Parts[nvar*2].c_str());

         std::vector<double> pt, errorlow, errorhigh;

         for(int i = 0; i + 2 < N; i = i + 3)
         {
            pt.push_back(atof(Parts[nvar*2+i+1].c_str()));
            errorlow.push_back(atof(Parts[nvar*2+i+2].c_str()));
            errorhigh.push_back(atof(Parts[nvar*2+i+3].c_str()));
         }
         PTBins.push_back(pt);
         ErrorLow.push_back(errorlow);
         ErrorHigh.push_back(errorhigh);
      }
   }

   in.close();

   Initialized = true;
}

std::vector<std::string> JetUncertainty::BreakIntoParts(std::string Line)
{
   std::stringstream str(Line);

   std::vector<std::string> Result;

   while(str)
   {
      std::string Temp = "";
      str >> Temp;
      if(Temp == "")
         continue;
      Result.push_back(Temp);
   }

   return Result;
}

bool JetUncertainty::CheckDefinition(std::string Line)
{
   for(int i = 0; i < (int)Line.size(); i++)
   {
      if(Line[i] == '{')
         return true;
      if(Line[i] == ' ')
         continue;
      return false;
   }

   return false;
}

std::string JetUncertainty::StripBracket(std::string Line)
{
   for(int i = 0; i < (int)Line.size(); i++)
   {
      if(Line[i] == '{' || Line[i] == '}')
      {
         Line.erase(Line.begin() + i);
         i = i - 1;
      }
   }

   return Line;
}

JetUncertainty::Type JetUncertainty::ToType(std::string Line)
{
   if(Line == "JetPt")    return TypeJetPT;
   if(Line == "JetEta")   return TypeJetEta;
   if(Line == "JetPhi")   return TypeJetPhi;
   if(Line == "JetA")     return TypeJetArea;
   if(Line == "Rho")      return TypeRho;

   std::cerr << "[JetUncertainty] Warning: variable type " << Line << " not found!" << std::endl;

   return TypeNone;
}

std::pair<double, double> JetUncertainty::GetUncertainty()
{
   if(Initialized == false)
      return std::pair<double, double>(-1, -1);

   int N = BinTypes.size();

   for(int iE = 0; iE < N; iE++)
   {
      bool InBin = true;

      for(int iB = 0; iB < (int)BinTypes[iE].size(); iB++)
      {
         double Value = GetValue(BinTypes[iE][iB]);
         if(Value < BinRanges[iE][iB*2] || Value > BinRanges[iE][iB*2+1])
            InBin = false;
      }

      if(InBin == false)
         continue;

      if(PTBins[iE].size() == 0)
         return std::pair<double, double>(-1, -1);

      double JetPT = GetValue(TypeJetPT);

      if(JetPT < PTBins[iE][0])
         return std::pair<double, double>(ErrorLow[iE][0], ErrorHigh[iE][0]);
      if(JetPT >= PTBins[iE][PTBins[iE].size()-1])
         return std::pair<double, double>(ErrorLow[iE][PTBins[iE].size()-1], ErrorHigh[iE][PTBins[iE].size()-1]);
      
      int Bin = 0;
      for(int i = 0; i < (int)PTBins[iE].size() - 1; i++)
      {
         if(JetPT >= PTBins[iE][i] && JetPT < PTBins[iE][i+1])
         {
            Bin = i;
            break;
         }
      }

      double Low = ErrorLow[iE][Bin] + (ErrorLow[iE][Bin+1] - ErrorLow[iE][Bin]) / (PTBins[iE][Bin+1] - PTBins[iE][Bin]) * (JetPT - PTBins[iE][Bin]);
      double High = ErrorHigh[iE][Bin] + (ErrorHigh[iE][Bin+1] - ErrorHigh[iE][Bin]) / (PTBins[iE][Bin+1] - PTBins[iE][Bin]) * (JetPT - PTBins[iE][Bin]);

      return std::pair<double, double>(Low, High);
   }

   return std::pair<double, double>(-1, -1);
}

double JetUncertainty::GetValue(Type T)
{
   if(T == TypeNone)      return 0;
   if(T == TypeJetPT)     return JetPT;
   if(T == TypeJetEta)    return JetEta;
   if(T == TypeJetPhi)    return JetPhi;
   if(T == TypeJetArea)   return JetArea;
   if(T == TypeRho)       return Rho;

   return -1;
}


