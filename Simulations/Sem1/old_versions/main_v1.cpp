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

int numberOfPeople = 9;
// Associate each man or woman ranker with an index
void generateNamesAndPreferences(bool print, ofstream& logFile);
vector<map<string, string> >  GaleShapley(ofstream& logFile);
vector<string> men;
vector<string> women;
map<string, vector<string> > menRanking;
map<string, vector<string> > womenRanking;
vector<string> initialRanking;
vector<string> temporaryRanking;
bool print = true;
int totalIdealMatches = 0;
int totalCanImprove =0;
int totalCannotImprove = 0;
int menStrictlyBetterOffByBetterWoman =0;
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

void betterWoman(string man, string woman,ofstream& logFile, vector<map<string, string> > original_matches){
    vector<map<string, string> > temporary_matches;
    vector<string> initialRanking = womenRanking[woman];
    int swapRanking = 1;
    vector<string> temporaryRanking = initialRanking;
    string swapMan = temporaryRanking[swapRanking];
    temporaryRanking.erase(temporaryRanking.begin() + swapRanking++);
    temporaryRanking.insert(temporaryRanking.begin(), swapMan);
    womenRanking[woman] = temporaryRanking;  
    temporary_matches = GaleShapley(logFile);  // Get the temporary matching
    string temporaryWoman = temporary_matches[0][man];  
    auto it = menRanking.find(man);
    auto originallyGoodPos = find(it->second.begin(), it->second.end(), woman);
    auto manipulatedPosition = find(it->second.begin(), it->second.end(), temporaryWoman);
    int originalIndex = distance(it->second.begin(), originallyGoodPos);
    int manipulatedIndex = distance(it->second.begin(), manipulatedPosition);
    if (manipulatedIndex < originalIndex) {
        logFile<<"better to "<<temporaryWoman;
        // betterOffCount++;  
         menStrictlyBetterOffByBetterWoman ++;
        womenRanking[woman] = initialRanking;
        return;
    }else{
        for(; swapRanking< temporaryRanking.size();swapRanking++){
            iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swapRanking);
            womenRanking[woman] = temporaryRanking;
            temporary_matches = GaleShapley(logFile);
            temporaryWoman = temporary_matches[0][man];  
            originallyGoodPos = find(it->second.begin(), it->second.end(), woman);
            manipulatedPosition = find(it->second.begin(), it->second.end(), temporaryWoman);
            originalIndex = distance(it->second.begin(), originallyGoodPos);
            manipulatedIndex = distance(it->second.begin(), manipulatedPosition);
            if (manipulatedIndex < originalIndex) {
                logFile<<"better to "<<temporaryWoman;
                 menStrictlyBetterOffByBetterWoman ++;
                // betterOffCount++;  
                womenRanking[woman] = initialRanking;
               return;
            }
        }
    }
    logFile<<"cannot make man better\n";
    womenRanking[woman] = initialRanking;
    

}


void Trial(ofstream& logFile, ofstream& resultFile) {
    menRanking.clear(); womenRanking.clear();
    generateNamesAndPreferences(print, logFile);
    vector<map<string, string> > original_matches ;
    original_matches= GaleShapley(logFile);
    vector<map<string, string> > temporary_matches;
     int countIdealMatches = 0;  // Metric 1: Men matched with their most ideal partner
    int countCannotImprove = 0;  // Metric 2: Men who cannot improve by manipulation
    map<string, int> improvementPossible;  // Metric 3: How many women can manipulate their lists to improve the man's match
    logFile<<"Original GS Matching\n";
    print_matches(original_matches,logFile);
    for(string man: men){
        logFile<<"\nMan "<<man;
        int betterOffCount = 0;
        if(original_matches[0][man]==  menRanking[man][0]){
            logFile<<": already best";
            countIdealMatches++;
            continue;
        }
        int currentMatchIndex = find(menRanking[man].begin(), menRanking[man].end(), original_matches[0][man]) - menRanking[man].begin();
        for(string woman: menRanking[man]){
            int womanIndex = find(menRanking[man].begin(), menRanking[man].end(), woman) - menRanking[man].begin();
            if (womanIndex <= currentMatchIndex) {
                // logFile<<woman<<": can't manipulate woman higher in the ranking list\n";
                logFile<<"\n Better woman "<<woman<<" ";
                betterWoman(man,woman,logFile,original_matches);
                continue;
            }
            vector<string> initialRanking = womenRanking[woman];
            int swapRanking = 1;
            vector<string> temporaryRanking = initialRanking;
            string swapMan = temporaryRanking[swapRanking];
            temporaryRanking.erase(temporaryRanking.begin() + swapRanking++);
            temporaryRanking.insert(temporaryRanking.begin(), swapMan);
            womenRanking[woman] = temporaryRanking;  
            temporary_matches = GaleShapley(logFile);  // Get the temporary matching
            string originalWoman = original_matches[0][man];  
            string temporaryWoman = temporary_matches[0][man];  
            auto it = menRanking.find(man);
            auto originalPosition = find(it->second.begin(), it->second.end(), originalWoman);
            auto manipulatedPosition = find(it->second.begin(), it->second.end(), temporaryWoman);
            int originalIndex = distance(it->second.begin(), originalPosition);
            int manipulatedIndex = distance(it->second.begin(), manipulatedPosition);
            if (manipulatedIndex < originalIndex) {
                logFile<<"\n better by "<<woman<<" to "<<temporaryWoman;
                betterOffCount++;  
                break;
            }else{
                // if( menRanking[man][1]== woman)
                //     logFile<<"If woman in #2 does, goes from  "<<originalWoman<<" to "<<temporaryWoman<<"\n";
                for(; swapRanking< temporaryRanking.size();swapRanking++){
                    iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swapRanking);
                    womenRanking[woman] = temporaryRanking;
                    temporary_matches = GaleShapley(logFile);
                    originalWoman = original_matches[0][man];  
                    temporaryWoman = temporary_matches[0][man];  
                    originalPosition = find(it->second.begin(), it->second.end(), originalWoman);
                    manipulatedPosition = find(it->second.begin(), it->second.end(), temporaryWoman);
                    originalIndex = distance(it->second.begin(), originalPosition);
                    manipulatedIndex = distance(it->second.begin(), manipulatedPosition);
                    if (manipulatedIndex < originalIndex) {
                        logFile<<"\n better by "<<woman<<" to "<<temporaryWoman;
                        betterOffCount++;  
                        break;
                    }
                    // if( menRanking[man][1]== woman)
                    // logFile<<"If woman in #2 does, goes from  "<<originalWoman<<" to "<<temporaryWoman<<"\n";
                }
            }
            womenRanking[woman] = initialRanking;
        }
        if (betterOffCount == 0) {
            countCannotImprove++;  // This man cannot improve by manipulating a woman lower than the woman he was matched to in Gale Shapley
            logFile<<": cannot improve";
            improvementPossible[man] = betterOffCount;
        }else{
            totalCanImprove++;
        }
        // Record the count of women lower than the man's original matching who can improve the man's match
        
    }
    // Add trial results to cumulative metrics
    totalIdealMatches += countIdealMatches;
    totalCannotImprove += countCannotImprove;
    
    resultFile << "Metric 1: " << countIdealMatches << " men matched with their most ideal partner.\n";
    resultFile << "Metric 2: " << countCannotImprove << " men cannot improve by manipulating a woman lower in their ranking than the match they got.\n";
    resultFile << "Metric 3:\n";
    // for (const auto& entry : improvementPossible) {
    //     resultFile << "Man " << entry.first << " can get better off with " << entry.second << " women manipulating their rankings.\n";
    // }
}





void generateNamesAndPreferences(bool pref_print, ofstream& logFile) {
    if (pref_print)
        logFile << "\nPreferences\n";
    std::random_device rd;
    std::mt19937 g(rd());
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
}

vector<map<string, string> > GaleShapley(ofstream& logFile) {
    map<string, string> woman_matched;  // map woman to man
    map<string, string> man_matched;    // map man to woman
    list<string> freeMen(men.begin(), men.end());  // initially, all men are free

    // Create quick lookup for women's preferences
    unordered_map<string, unordered_map<string, int> > womenRank;
    for (const auto& woman : womenRanking) {
        const string& womanName = woman.first;
        const vector<string>& rankings = woman.second;
        for (int i = 0; i < rankings.size(); ++i) {
            womenRank[womanName][rankings[i]] = i;  // rank is the index in the preference list
        }
    }

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
                // Compare currentMan and otherMan using the woman's preference
                if (womenRank[woman][currentMan] < womenRank[woman][otherMan]) {
                    woman_matched[woman] = currentMan;
                    man_matched[currentMan] = woman;
                    freeMen.push_back(otherMan);  // the other man is now free
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
    
    for (int i = 0; i < numberOfPeople; i++) {
        men.push_back(to_string(i + 1));  // Men are still numbered from 1 to 10
        women.push_back(string(1, 'A' + i));  // Women are assigned letters from 'A' to 'J'
    }
    int trials =100;
    for(int i=0; i<trials;i++){
        logFile<<"Trial #"<<i<<"\n";
        resultFile<<"Trial #"<<i<<"\n";
        Trial(logFile,resultFile);
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