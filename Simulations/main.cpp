#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <list>
#include <algorithm>
#include <random>
#include <cstdlib> 
#include <fstream>

using namespace std;

int numberOfPeople = 10;
// Associate each man or woman ranker with an index
void generateNamesAndPreferences(bool print, ofstream& logFile);
vector<map<string, string>>  GaleShapley(ofstream& logFile);
vector<string> men;
vector<string> women;
map<string, vector<string>> menRanking;
map<string, vector<string>> womenRanking;
vector<string> initialRanking;
vector<string> temporaryRanking;
bool print = true;

int accross_trial_men_with_incentive = 0;
int accross_trial_men_with_best_partner = 0;

void Trial(ofstream& logFile, ofstream& resultFile) {
    generateNamesAndPreferences(print, logFile);
    vector<map<string, string>> original_matches = GaleShapley(logFile);
    vector<map<string, string>> temporary_matches;
    int men_with_incentive = 0, men_with_best_partner = 0;

    for (string chosenMan : men) {

        if (original_matches[0][chosenMan] == menRanking[chosenMan][0]) {
            men_with_best_partner++;
            continue;
        }
        if (print)
            logFile << "Man " << chosenMan << " " << original_matches[0][chosenMan] << "\n Possible Accomplices: \n";

        int possible_accomplices = 0;
        bool found_better = false;

        for (string& chosenWoman : menRanking[chosenMan]) {
            vector<string> initialRanking = womenRanking[chosenWoman];
            int swapRanking = find(initialRanking.begin(),
                initialRanking.end(), original_matches[1][chosenWoman]) - initialRanking.begin() + 1;
            vector<string> temporaryRanking = initialRanking;
            if (swapRanking == initialRanking.size())
                continue;
            string swapMan = temporaryRanking[swapRanking];
            temporaryRanking.erase(temporaryRanking.begin() + swapRanking);
            temporaryRanking.insert(temporaryRanking.begin(), swapMan);

            womenRanking[chosenWoman] = temporaryRanking;

            temporary_matches = GaleShapley(logFile);
            int man_difference = find(menRanking[chosenMan].begin(),
                menRanking[chosenMan].end(), original_matches[0][chosenMan])
                - find(menRanking[chosenMan].begin(),
                    menRanking[chosenMan].end(), temporary_matches[0][chosenMan]);

            if (man_difference > 0) {
                if (print) {
                    logFile << "  " << chosenWoman << " matched to " << temporary_matches[1][chosenWoman]
                        << " was matched to " << original_matches[1][chosenWoman]
                        << "\n  Man matched to " << temporary_matches[0][chosenMan] << "\n";
                }

                found_better = true;
                men_with_incentive++;
                womenRanking[chosenWoman] = initialRanking;
            }

            for (int swap = swapRanking + 1; swap < temporaryRanking.size(); swap++) {
                iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swap);
                womenRanking[chosenWoman] = temporaryRanking;

                temporary_matches = GaleShapley(logFile);
                man_difference = find(menRanking[chosenMan].begin(),
                    menRanking[chosenMan].end(), original_matches[0][chosenMan])
                    - find(menRanking[chosenMan].begin(),
                        menRanking[chosenMan].end(), temporary_matches[0][chosenMan]);

                if (man_difference > 0) {
                    if (print) {
                        logFile << "  " << chosenWoman << " matched to " << temporary_matches[1][chosenWoman]
                            << " was matched to " << original_matches[1][chosenWoman]
                            << "\n  Man matched to " << temporary_matches[0][chosenMan] << "\n";
                    }

                    found_better = true;
                    men_with_incentive++;
                    womenRanking[chosenWoman] = initialRanking;
                    break;
                }
            }
            womenRanking[chosenWoman] = initialRanking;
        }
    }

    resultFile << men_with_incentive << " men were strictly better off in this trial.\n";
    accross_trial_men_with_incentive += men_with_incentive;
    accross_trial_men_with_best_partner += men_with_best_partner;
}

void generateNamesAndPreferences(bool print, ofstream& logFile) {
    if (print)
        logFile << "Preferences\n";
    std::random_device rd;
    std::mt19937 g(rd());

    // Generate men's and women's names
    for (int i = 0; i < numberOfPeople; i++) {
        men.push_back(to_string(i + 1));
        women.push_back(string(1, 'A' + i % 26));
    }

    // Generate random preferences for each man
    for (auto& man : men) {
        vector<string> preferences = women;
        shuffle(preferences.begin(), preferences.end(), g);
        menRanking[man] = preferences;
        if (print) {
            logFile << man << " : ";
            for (auto& w : preferences)
                logFile << w << " ";
            logFile << "\n";
        }
    }

    // Generate random preferences for each woman
    for (auto& woman : women) {
        vector<string> preferences = men;
        shuffle(preferences.begin(), preferences.end(), g);
        womenRanking[woman] = preferences;
        if (print) {
            logFile << woman << " : ";
            for (auto& w : preferences)
                logFile << w << " ";
            logFile << "\n";
        }
    }
}

vector<map<string, string>>  GaleShapley(ofstream& logFile) {
    map<string, string> woman_matched;  // map woman to man
    map<string, string> man_matched;    // map man to woman
    list<string> freeMen(men.begin(), men.end());  // initially, all men are free

    while (!freeMen.empty()) {
        string currentMan = freeMen.front();
        freeMen.pop_front();

        vector<string> currentManPrefers = menRanking[currentMan];

        for (string& woman : currentManPrefers) {
            if (woman_matched.find(woman) == woman_matched.end()) {  // woman is not matched
                woman_matched[woman] = currentMan;
                man_matched[currentMan] = woman;
                break;
            } else {
                string otherMan = woman_matched[woman];
                vector<string> currentWomanRanking = womenRanking[woman];

                if (find(currentWomanRanking.begin(), currentWomanRanking.end(), currentMan) <
                    find(currentWomanRanking.begin(), currentWomanRanking.end(), otherMan)) {
                    woman_matched[woman] = currentMan;
                    man_matched[currentMan] = woman;
                    freeMen.push_back(otherMan);
                    break;
                }
            }
        }
    }

    return { man_matched, woman_matched };
}

int main(int argc, char* argv[]) {
    if (argc == 2)
        numberOfPeople = atoi(argv[1]);

    ofstream resultFile("trial_results.txt");
    ofstream logFile("log_file.txt");

    int num_trials = 1;
    for (int i = 1; i <= num_trials; i++) {
        if (print)
            logFile << "Trial #" << i << " ";
        Trial(logFile, resultFile);
    }

    resultFile.close();
    logFile.close();

    return 0;
}
