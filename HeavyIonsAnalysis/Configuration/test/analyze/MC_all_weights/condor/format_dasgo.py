def format_dasgoclient_output(input_file, output_file):
    """
    Reads a list of files from input_file, formats them with quotes and commas,
    and writes the formatted list to output_file, excluding the comma on the last line.

    Args:
        input_file (str): Path to the input file containing the list of files.
        output_file (str): Path to the output file to write the formatted list.
    """
    try:
        with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
            lines = infile.readlines()
            for i, line in enumerate(lines):
                file_path = line.strip()
                if file_path:
                    if i < len(lines) - 1:  # Check if it's not the last line
                        formatted_line = f'"{file_path}",\n'
                    else:  # Last line, no comma
                        formatted_line = f'"{file_path}"\n'
                    outfile.write(formatted_line)
        print(f"Formatted file list written to {output_file}")

    except FileNotFoundError:
        print(f"Error: Input file '{input_file}' not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

# Example usage:
input_file = "list_MC_ZMM_all"
output_file = "formatted_list_MC_ZMM_all.txt"
format_dasgoclient_output(input_file, output_file)
