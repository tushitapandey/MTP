#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include <map>
#include <list>
#include <algorithm>
#include <random>
#include <cstdlib> 
#include <fstream>
#include<set>
using namespace std;

int numberOfPeople = 26;
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
bool result_print= true;

int accross_trial_men_with_incentive = 0;
int accross_trial_men_with_best_partner = 0;
int accross_trial_men_with_who_do_not_have_a_better_partner=0;
void Trial(ofstream& logFile, ofstream& resultFile) {
    menRanking.clear(); womenRanking.clear();
    generateNamesAndPreferences(print, logFile);
    vector<map<string, string>> original_matches = GaleShapley(logFile);
    vector<map<string, string>> temporary_matches;
    int men_with_best_partner = 0;
    set<string> men_with_incentive;
    for (string chosenMan : men) {

        if (original_matches[0][chosenMan] == menRanking[chosenMan][0]) {
            men_with_best_partner++;
            continue;
        }
        // if (print)
        //     logFile << "Man " << chosenMan << " " << original_matches[0][chosenMan] <<"\n ";

        // int possible_accomplices = 0;
        bool found_better = false;

        for (string& chosenWoman : menRanking[chosenMan]) {
            if(chosenWoman == menRanking[chosenMan][0]){
                continue;
            }
            if(found_better==true){
                break;
            }
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
                     womenRanking[chosenWoman] = initialRanking;
                     if (result_print || print) {
                        // logFile<<"\nMan "<<chosenMan<<" goes from "<<original_matches[0][chosenMan] << " to "<<  temporary_matches[0][chosenMan];
                        // logFile << " after manipulation by Woman " << chosenWoman << " who goes from " << original_matches[1][chosenWoman]
                        //     << " to " << temporary_matches[1][chosenWoman]<<"\n";
                        logFile<< "Man difference is "<<man_difference<<"\n";


                        logFile<< "Woman difference is "<<find(womenRanking[chosenWoman].begin(),
                    womenRanking[chosenWoman].end(), original_matches[1][chosenWoman])
                    - find(womenRanking[chosenWoman].begin(),
                        womenRanking[chosenWoman].end(), temporary_matches[1][chosenWoman])<<"\n";
                        
                    }
                    found_better = true;
                    men_with_incentive.insert(chosenMan);
                    break;
                }

            for (int swap = swapRanking + 1; !found_better&&  swap < temporaryRanking.size(); swap++) {
                iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swap);
                womenRanking[chosenWoman] = temporaryRanking;

                temporary_matches = GaleShapley(logFile);
                man_difference = find(menRanking[chosenMan].begin(),
                    menRanking[chosenMan].end(), original_matches[0][chosenMan])
                    - find(menRanking[chosenMan].begin(),
                        menRanking[chosenMan].end(), temporary_matches[0][chosenMan]);

                if (man_difference > 0) {
                     womenRanking[chosenWoman] = initialRanking;
                     if (result_print || print) {
                        // logFile<<"\nMan "<<chosenMan<<" goes from "<<original_matches[0][chosenMan] << " to "<<  temporary_matches[0][chosenMan];
                        // logFile << " after manipulation by Woman " << chosenWoman << " who goes from " << original_matches[1][chosenWoman]
                        //     << " to " << temporary_matches[1][chosenWoman]<<"\n";
                        logFile<< "Man difference is "<<man_difference<<"\n";


                        logFile<< "Woman difference is "<<find(womenRanking[chosenWoman].begin(),
                    womenRanking[chosenWoman].end(), original_matches[1][chosenWoman])
                    - find(womenRanking[chosenWoman].begin(),
                        womenRanking[chosenWoman].end(), temporary_matches[1][chosenWoman])<<"\n";
                        
                    }
                    found_better = true;
                    men_with_incentive.insert(chosenMan);
                    break;
                }
            }
            womenRanking[chosenWoman] = initialRanking;
        }
    }
    if(result_print)
        resultFile << men_with_incentive.size() << " men were strictly better off in this trial.\n";
    accross_trial_men_with_who_do_not_have_a_better_partner += (numberOfPeople- men_with_best_partner - men_with_incentive.size());
    accross_trial_men_with_incentive += men_with_incentive.size();
    accross_trial_men_with_best_partner += men_with_best_partner;
    
}

void generateNamesAndPreferences(bool loc_print, ofstream& logFile) {
    loc_print=!print;
    if (print)
        logFile << "\nPreferences\n";
    std::random_device rd;
    std::mt19937 g(rd());

    // Generate random preferences for each man
    for (auto& man : men) {
        vector<string> preferences = women;
        shuffle(preferences.begin(), preferences.end(), g);
        menRanking[man] = preferences;
        if (loc_print) {
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
        if (loc_print) {
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

    ofstream resultFile("results.txt");
    ofstream logFile("log_file.txt");
    ofstream finalResultFile("final_results.txt");
    numberOfPeople=40;
        // Generate men's and women's names
    for (int i = 0; i < numberOfPeople; i++) {
        men.push_back(to_string(i + 1));
        women.push_back(to_string(i + 1));
    }
    int num = numberOfPeople;
    for(;numberOfPeople<101; numberOfPeople+=20)
    {
        accross_trial_men_with_incentive = 0;
        accross_trial_men_with_best_partner = 0;
        accross_trial_men_with_who_do_not_have_a_better_partner=0;
        resultFile <<"Number of men: "<<numberOfPeople<<"\n";
        logFile<<"Number of men: "<<numberOfPeople<<"\n";
        int num_trials = 100; 
        for (int i = 1; i <= num_trials; i++) {
            if (result_print){
                resultFile << "Trial #" << i << " ";
                logFile << "Trial #" << i << " ";
            }
            
            Trial(logFile, resultFile);
        }
        finalResultFile<<"Number of men: "<<numberOfPeople<<"\n";
        finalResultFile<< (double)(100*accross_trial_men_with_who_do_not_have_a_better_partner)/(numberOfPeople*num_trials) <<"\% of men have no incentive to look for an accomplice even though ther're not matched to their first preference\n";
        finalResultFile<< (double)(100*accross_trial_men_with_incentive)/(numberOfPeople*num_trials) <<"\% of men have an incentive to look for an accomplice\n";
        finalResultFile<< (double)accross_trial_men_with_best_partner/(numberOfPeople*num_trials) *100 <<"\% of men already get their top partner in the original Gale Shapley Algorithm\n";
         for(int i=0;i<20;i++){
            num++;
            men.push_back(to_string(num));
            women.push_back(to_string(num));
        }
    }
     
     resultFile.close();
    logFile.close();

    return 0;
}
