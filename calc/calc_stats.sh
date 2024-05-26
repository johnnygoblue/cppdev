#!/bin/bash

############################################################################
# This script does two things
# 1. Filter the logs in-place with only the information we really need (new order,
#    cancel order, match report) for the specific strategy we want stats on.
# 2. Run the corresponding program to calculate stats based on the pre-processed 
#    log files.
#
# Prerequisites:
# The log files under the designated dir must already be processed on Windows side
############################################################################

# Path to the log directory (all files in this dir will be affected)
log_dir="log"

# Check if the log directory exists
if [ ! -d "$log_dir" ]; then
    echo "Error: Log directory not found: $log_dir"
    exit 1
fi

# Function to apply filters and save to temporary file
apply_filters() {
    local input_file="$1"
    local temp_file="$input_file.temp"

    # grep only the new/cancel/match lines for strategy C70
    grep -a -E '.*OrderUpdate.*C70|.*OrderReport.*C70|.*\[MatchReport\].*IntraDayOdd.*Buy' "$input_file" > "$temp_file" || { echo "Error: Grep filter failed."; exit 1; }

    # Overwrite the original file with the filtered content
    mv "$temp_file" "$input_file" || { echo "Error: Failed to rename temporary file."; exit 1; }
}

check_and_rename_filename() {
    local filename="$1"
    # check if filename matches YYYYMMDD.ibfs
    if [[ "$filename" =~ ^([0-9]{8})\.ibfs$ ]]; then
        local date_part="${BASH_REMATCH[1]}"
        # validate date
        if date -d "${date_part:0:4}-${date_part:4:2}-${date_part:6:2}" >/dev/null 2>&1; then
            echo "The filename '$filename' is in the correct format"
            return 0
        else
            echo "Invalid date in filename '$filename'"
            return 1
        fi
    else
        # check if filename contains a valid date
        if [[ "$filename" =~ ([0-9]{8}) ]]; then
            local date_part="${BASH_REMATCH[1]}"
            # validate date_part
            if date -d "${date_part:0:4}-${date_part:4:2}-${date_part:6:2}" >/dev/null 2>&1; then
                local new_filename="${date_part}.ibfs"
                mv "$filename" "$new_filename"
                echo "Renamed '$filename' to '$new_filename'"
                return 0
            else
                echo "Invalid date in filename '$filename'"
                return 1
            fi
        else
            echo "The filename '$filename' does not contain a valid date"
            return 1
        fi
    fi
}

# Loop through all files in the log directory
cd "$log_dir"
for file in *; do
    # Check if there are any files
    if [ -e "$file" ]; then
        # Apply filters to the file
        apply_filters "$file"
        check_and_rename_filename "$file"
    fi
done
cd ..

# Execute the program to collect stats with the appropriate arguments
for file in "$log_dir"/*; do
    if [ -e "$file" ]; then
        ./bin/calc -f "$file" -s 1 -o "sql/oddlot.db"
    fi
done

# Clean up temporary files (optional)
rm -f "$log_dir"/*.temp
