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


vector<string> men;
vector<string> women;
bool print = true;
int totalIdealMatches = 0;
int totalCanImprove =0;
int totalCannotImprove = 0;
int menStrictlyBetterOffByBetterWoman =0;
vector<map<string, vector<string> > > global_map;
void print_matches(vector<map<string, string> > matching, ofstream& logFile) {

    // Print Man to Woman Matches
    // logFile << "Man to Woman Matches: " << endl;
    for (const auto& manMatch : matching[0]) {
        logFile << "Man " << manMatch.first << " : " << manMatch.second << endl;
    }
    // // Print Woman to Man Matches
    // logFile << "\nWoman to Man Matches: " << endl;
    // for (const auto& womanMatch : matching[1]) {
    //     logFile << "Woman " << womanMatch.first << " : " << womanMatch.second << endl;
    // }
}

vector<map<string, vector<string> > > generateNamesAndPreferences(bool pref_print, ofstream& logFile) {
    map<string, vector<string> > menRanking;
    map<string, vector<string> > womenRanking;
    if (pref_print)
        logFile << "\nPreferences\n";
    random_device rd;
    mt19937 g(rd());
    for (auto& man : men) {
        vector<string> preferences = women;
        shuffle(preferences.begin(), preferences.end(), g);
        menRanking[man] = preferences;
        if (pref_print) {
            logFile << man << " : ";
            for (auto& w : preferences)
                logFile << w << " ";
            logFile << "\n";
        }
    }
    for (auto& woman : women) {
        vector<string> preferences = men;
        shuffle(preferences.begin(), preferences.end(), g);
        womenRanking[woman] = preferences;
        if (pref_print) {
            logFile << woman << " : ";
            for (auto& w : preferences)
                logFile << w << " ";
            logFile << "\n";
        }
    }
    return { menRanking, womenRanking };
}


vector<map<string, string>> GaleShapley( ofstream& logFile,map<string, vector<string>>& menRanking,
                                         map<string, vector<string>>& womenRanking
                                        ) {
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

void deal(ofstream& logFile, string woman, string og_woman, 
                            vector<map<string, string> > original_matches, 
                            vector<map<string, string> > temp_matches,  
                            vector<string> og_woman_rank, 
                            vector<string> initialRanking,
                            map<string, vector<string>> menRanking,
                            map<string, vector<string>> womenRanking,
                            vector<string> temporaryRanking){
    
    string ogManWoman2, ogManWoman1, newManWoman2, newManWoman1;
   
    int man_diff, w1_diff, w2_diff;

    // vector<map<string, string> > temporary_matches_new=temp_matches;
    // vector<map<string, string> > temporary_matches_gs=GaleShapley(logFile,menRanking,
    //                                       womenRanking);
    vector<map<string, string> > temporary_matches_new=GaleShapley(logFile,menRanking,
                                          womenRanking);
   
    

    vector<string> womenWithNegativeDiff;

    for (size_t i = 0; i < original_matches.size(); ++i) {
        string ogManWoman = original_matches[0][woman];
        string newManWoman = temp_matches[0][woman];

        auto originalPosition = find(global_map[woman].begin(), global_map[woman].end(), ogManWoman);
        auto manipulatedPosition = find(global_map[woman].begin(), global_map[woman].end(), newManWoman);

        if (originalPosition != global_map[woman].end() && manipulatedPosition != global_map[woman].end()) {
            int originalIndex = distance(global_map[woman].begin(), originalPosition);
            int manipulatedIndex = distance(global_map[woman].begin(), manipulatedPosition);

            int w2_diff = manipulatedIndex - originalIndex;

            if (w2_diff < 0) {
                womenWithNegativeDiff.push_back(newManWoman);
            }
        }
    }

    
}
void woman_3(string og_woman, 
            vector<string> og_woman_rank, 
            vector<map<string, string> > original_matches,  
            ofstream& logFile, ofstream& resultFile, 
            map<string, vector<string>> menRanking,
            map<string, vector<string>> womenRanking){
    vector<map<string, string> > temporary_matches =  GaleShapley(logFile,menRanking,
                                          womenRanking);
    
    for(string woman: women){
        if(woman==og_woman){
            continue;
        }
        else{
            vector<string> initialRanking = global_map[woman];
            deal(logFile,woman,og_woman,original_matches,temporary_matches,og_woman_rank, initialRanking,menRanking,womenRanking,womenRanking[woman]);
            auto iterator= find(womenRanking[woman].begin(), womenRanking[woman].end(), original_matches[1][woman]);
            int swapRanking =  distance(womenRanking[woman].begin(), iterator) ;
            swapRanking =2;
            if(swapRanking == womenRanking[woman].size())
                continue;
            vector<string> temporaryRanking = initialRanking;
            string swapMan = temporaryRanking[swapRanking];
            temporaryRanking.erase(temporaryRanking.begin() + swapRanking++);
            temporaryRanking.insert(temporaryRanking.begin(), swapMan);
            womenRanking[woman] = temporaryRanking;  
            deal(logFile,woman,og_woman,original_matches,temporary_matches,og_woman_rank, initialRanking,menRanking,womenRanking,temporaryRanking);
            for(; swapRanking< temporaryRanking.size() ;swapRanking++){
                    iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swapRanking);
                    womenRanking[woman] = temporaryRanking;  
        
                    deal(logFile,woman,og_woman,original_matches,temporary_matches,og_woman_rank, initialRanking,menRanking,womenRanking,temporaryRanking);
            }
            womenRanking[woman] = initialRanking;
        }
    }
}

void Trial_3(ofstream& logFile, ofstream& resultFile, 
                map<string, vector<string>> menRanking,
                map<string, vector<string>> womenRanking){
    vector<map<string, string> > original_matches ;
    original_matches= GaleShapley(logFile,menRanking,
                                          womenRanking);
    vector<map<string, string> > temporary_matches;
    logFile<<"Original GS Matching\n";
    print_matches(original_matches,logFile);
    for(string woman: women){
        vector<string> initialRanking = womenRanking[woman];
            auto iterator= find(womenRanking[woman].begin(), womenRanking[woman].end(), original_matches[1][woman]);
            int swapRanking =  distance(womenRanking[woman].begin(), iterator) ;
            swapRanking =2;
            if(swapRanking == womenRanking[woman].size())
                continue;
            vector<string> temporaryRanking = initialRanking;
            string swapMan = temporaryRanking[swapRanking];
            temporaryRanking.erase(temporaryRanking.begin() + swapRanking++);
            temporaryRanking.insert(temporaryRanking.begin(), swapMan);
            womenRanking[woman] = temporaryRanking;  
            for(int i=0;i<9;i++){
                if(womenRanking[woman][i]!=temporaryRanking[i])
                cout<<"bad\n";
            };  
            woman_3(woman,initialRanking, original_matches, logFile, resultFile,menRanking,womenRanking);
            for(; swapRanking< temporaryRanking.size() ;swapRanking++){
                iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swapRanking);
                womenRanking[woman] = temporaryRanking;  
                woman_3(woman,initialRanking, original_matches, logFile, resultFile,menRanking,womenRanking);
            }
            womenRanking[woman] = initialRanking;
    }

}

int main(int argc, char* argv[]) {
    if (argc == 2)
        numberOfPeople = atoi(argv[1]);

    ofstream resultFile("results.txt");
    ofstream logFile("log_file.txt");
    ofstream finalResultFile("final_results.txt");
    int numberOfPeople = 9;
    for (int i = 0; i < numberOfPeople; i++) {
        men.push_back(to_string(i + 1));  // Men are still numbered from 1 to 10
        women.push_back(string(1, 'A' + i));  // Women are assigned letters from 'A' to 'J'
    }
    int trials =10;
    vector<map<string, vector<string> > > m = generateNamesAndPreferences(false,logFile);
    global_map=m;
    for(int i=0; i<trials;i++){
        logFile<<"Trial #"<<i<<"\n";
        resultFile<<"Trial #"<<i<<"\n";
        Trial_3(logFile,resultFile,m[0],m[1]);
        // galeShapleyOnCustom(logFile);
        logFile<<"\n\n";
        
        
    }
    resultFile<<"\n\n";
        finalResultFile << "After " << trials  <<" trials:\n";
        finalResultFile << "Total Metric 1: " << 100.0*totalIdealMatches/(numberOfPeople*trials) << "% men matched with their most ideal partner.\n";
        finalResultFile << "Total Metric 2: " << 100.0*totalCannotImprove/(numberOfPeople*trials) << "% men cannot improve by manipulating a woman lower in their ranking than the match they got.\n";
        finalResultFile << "Total Metric 3: " << 100.0*totalCanImprove/(numberOfPeople*trials) << "% men can improve by manipulating a woman lower in their ranking than the match they got.\n";
        finalResultFile << "Total Metric 4: " << 100.0*menStrictlyBetterOffByBetterWoman /(numberOfPeople*trials) << "% men can improve to a woman higher than the lying woman in their ranking.\n";
    return 0;
}
