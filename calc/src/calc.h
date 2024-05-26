#include <vector>
#include <string>
#include <optional>

using std::string;
using std::vector;

const char sep = ' ';

int exampleFunction(int a, int b)
{
    return a + b;
}

std::optional<string> getNextNthEntry(const string& input, const string& key, int n) {
    size_t pos = 0;
    while (pos != string::npos) {
        size_t start = input.find_first_not_of(sep, pos);
        if (start == string::npos) break;
        
        size_t end = input.find_first_of(sep, start);
        if (end == string::npos) break;
        string word = input.substr(start, end - start);
        if (word == key) {
            // skip the first n-1 entries
            for (int i = 0; i < n; ++i) {
                size_t next_start = input.find_first_not_of(sep, end);
                if (next_start == string::npos) break;

                size_t next_end = input.find_first_of(sep, next_start);
                if (next_end == string::npos) input.size()-1;
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

vector<string> genCheckPoints(const string& startTime, const string& endTime, int intervalSeconds) {
    std::vector<string> checkpoints;

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
