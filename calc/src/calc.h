#include <vector>
#include <string>
#include <optional>

const char sep = ' ';

// From an input string and given a key, find the next n-th item (space-separated) after the key
// 
// For exmple, getNextNthEntry("This is a test", "is", 2) returns "test"
// 
// Arguments: 
//      <input> is a space-separated string
//      <key> is a key that's contained within the input (if not, nullopt is returned)
//      <n> is the next nth item to find
std::optional<std::string> getNextNthEntry(const std::string& input, const std::string& key, int n) {
    size_t pos = 0;
    while (pos != std::string::npos) {
        size_t start = input.find_first_not_of(sep, pos);
        if (start == std::string::npos) break;
        
        size_t end = input.find_first_of(sep, start);
        if (end == std::string::npos) break;
        std::string word = input.substr(start, end - start);
        if (word == key) {
            // skip the first n-1 entries
            for (int i = 0; i < n; ++i) {
                size_t next_start = input.find_first_not_of(sep, end);
                if (next_start == std::string::npos) break;

                size_t next_end = input.find_first_of(sep, next_start);
                if (next_end == std::string::npos) input.size()-1;
                if (i == n - 1) {
                    return input.substr(next_start, next_end - next_start);
                }
                end = next_end;
            }
        }
        pos = end;
    }
    return std::nullopt;
}

// Given startTime and endTime in HH:MM:SS format, return a list of timestamps (also in HH:MM:SS)
// starting from startTime and no later than endTime with each timestamp being
// intervalSeconds later than the previous one (ascending).
//
// For example, genCheckPoints("09:00:00", "09:00:05", 2) returns 
// {"09:00:00", "09:00:02", "09:00:04"}
//
// Arguments:
//      <startTime> is the start time in HH:MM:SS
//      <endTime> is the end time in HH:MM:SS
//      <intervalSeconds> is the number seconds from current check point to the next
std::vector<std::string> genCheckPoints(const std::string& startTime, const std::string& endTime, int intervalSeconds) {
    std::vector<std::string> checkpoints;

    // Parse start time
    int startHour, startMinute, startSecond;
    sscanf(startTime.c_str(), "%d:%d:%d", &startHour, &startMinute, &startSecond);

    // Parse end time
    int endHour, endMinute, endSecond;
    sscanf(endTime.c_str(), "%d:%d:%d", &endHour, &endMinute, &endSecond);

    // Convert start time to total seconds
    int totalSecondsStart = startHour * 3600 + startMinute * 60 + startSecond;

    // Convert end time to total seconds
    int totalSecondsEnd = endHour * 3600 + endMinute * 60 + endSecond;

    // Generate checkpoints
    for (int totalSeconds = totalSecondsStart; totalSeconds <= totalSecondsEnd; totalSeconds += intervalSeconds) {
        int hour = totalSeconds / 3600;
        int minute = (totalSeconds % 3600) / 60;
        int second = totalSeconds % 60;

        char buffer[9];
        sprintf(buffer, "%02d:%02d:%02d", hour, minute, second);
        checkpoints.push_back(buffer);
    }

    return checkpoints;
}    
