
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
#include <cctype>
#include <sstream>
#include <string>
using namespace std;
vector<string> men;
vector<string> women;
bool detailed_log = false;


int n = 20;
void print_matches(vector<map<string, string> >& matching, ofstream& logFile) {
    for ( auto& manMatch : matching[0]) {
        logFile << "Man " << manMatch.first << " : " << manMatch.second << endl;
    }
    //logFile<<"\n";

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


vector<map<string, string>> GaleShapley( map<string, vector<string>>& menRanking,
                                          map<string, vector<string>>& womenRanking,
                                         ofstream& logFile) {
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

void moveToPosition(vector<string>& ranking, int currentPos, int targetPos) {
    swap(ranking[currentPos], ranking[targetPos]);
    // cout << "Moved to position " << targetPos + 1 << ": ";
    
    // cout << endl;
}

// If a man can get a woman to lie for him to get better, is it possible that the woman does it in lesser number of inversions.
void Trial1(map<string, vector<string>>  menRanking, map<string, vector<string>> womenRanking,ofstream& logFile){
    // string man = men[0];
    // string woman = women[0];
    vector<map<string, string> > original_matches= GaleShapley(menRanking, womenRanking, logFile);
    
    vector<map<string, string> > temporary_matches;//= GaleShapley(menRanking, womenRanking, logFile);
    vector<map<string, string> > temporary_matches_temp;
    vector<string> original_pref_w1;
    vector<string> original_pref_w2;
    vector<string> preferences;
    vector<string> preferences_2; 
    for (auto& man: men){
        if(detailed_log)
            logFile<<"For man "<<man<<"\n";
        if(original_matches[0][man]==  menRanking[man][0]){
            if(detailed_log)
                logFile<<" Already best match\n";
            continue;
        }
        int currentMatchIndex = find(menRanking[man].begin(), menRanking[man].end(), original_matches[0][man]) - menRanking[man].begin();
        for(auto& woman: women){
            if(detailed_log)
                logFile<<" Lying woman : "<<woman<<"\n";
            int womanIndex = find(menRanking[man].begin(), menRanking[man].end(), woman) - menRanking[man].begin();
            if (womanIndex < currentMatchIndex) {
                continue;
            }
            vector<string> initialRanking = womenRanking[woman];
            auto iterator= find(womenRanking[woman].begin(), womenRanking[woman].end(), original_matches[1][woman]);
            int swapRanking =  0;
            vector<string> temporaryRanking = initialRanking;
            string swapMan = temporaryRanking[swapRanking];
            temporaryRanking.erase(temporaryRanking.begin() + swapRanking++);
            temporaryRanking.insert(temporaryRanking.begin(), swapMan);
            womenRanking[woman] = temporaryRanking;  
            temporary_matches = GaleShapley(menRanking, womenRanking, logFile);  // Get the temporary matching
            string originalWoman = original_matches[0][man];  
            string temporaryWoman = temporary_matches[0][man];  
            auto it = menRanking.find(man);
            auto originallyGoodPos = find(it->second.begin(), it->second.end(), woman);
            auto manipulatedPosition = find(it->second.begin(), it->second.end(), temporaryWoman);
            int originalIndex = distance(it->second.begin(), originallyGoodPos);
            int manipulatedIndex = distance(it->second.begin(), manipulatedPosition);
            if (manipulatedIndex < originalIndex) {
                string moved_man = temporaryRanking[0];
                    if(detailed_log)
                        logFile<<"  "<<moved_man<<" "<<manipulatedIndex<<"\n";
                    vector<string> userRanking = initialRanking;
                    vector<string>::iterator temp_it = find(userRanking.begin(), userRanking.end(), moved_man);
                    if (temp_it != userRanking.end()) {
                        int pos = distance(userRanking.begin(), temp_it);
                        // Move the element one position at a time to the 2nd position (index 1)
                        for (int i = pos; i > 1; --i) {
                            moveToPosition(userRanking, i, i - 1);
                            womenRanking[woman] = userRanking;
                            temporary_matches_temp = GaleShapley(menRanking, womenRanking, logFile);  // Get the temporary matching
                            temporaryWoman = temporary_matches_temp[0][man];  
                            auto new_manipulatedPosition = find(it->second.begin(), it->second.end(), temporaryWoman);
                            if(manipulatedPosition > new_manipulatedPosition){
                                
                                logFile<<"Success! with difference "<< manipulatedPosition- new_manipulatedPosition<<" New ranking for the woman "<<woman<<" \n";
                                for (int i = 0; i < userRanking.size(); ++i) {
                                    logFile << userRanking[i] << " ";
                                }
                                logFile<<"Original matches \n";
                                print_matches(original_matches,logFile);
                                logFile<<"\nManipulation 1 matches\n";
                                print_matches(temporary_matches,logFile);
                                logFile<<"New matches ("<<man<<") should be getting the same match or better\n";
                                print_matches(temporary_matches_temp, logFile);

                                break;
                            }
                        }
                    }
                womenRanking[woman] = initialRanking;
             
            }else{
            for(; swapRanking< temporaryRanking.size();swapRanking++){
                iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swapRanking);
                womenRanking[woman] = temporaryRanking;
                temporary_matches = GaleShapley(menRanking, womenRanking, logFile);
                temporaryWoman = temporary_matches[0][man];  
                originallyGoodPos = find(it->second.begin(), it->second.end(), woman);
                manipulatedPosition = find(it->second.begin(), it->second.end(), temporaryWoman);
                originalIndex = distance(it->second.begin(), originallyGoodPos);
                manipulatedIndex = distance(it->second.begin(), manipulatedPosition);
                if (manipulatedIndex < originalIndex) 
                {
                    string moved_man = temporaryRanking[0];
                    vector<string> userRanking = initialRanking;
                    vector<string>::iterator temp_it = find(userRanking.begin(), userRanking.end(), moved_man);
                    if (temp_it != userRanking.end()) {
                        int pos = distance(userRanking.begin(), temp_it);
                        // Move the element one position at a time to the 2nd position (index 1)
                        for (int i = pos; i > 1; --i) {
                            moveToPosition(userRanking, i, i - 1);
                            womenRanking[woman] = userRanking;
                            temporary_matches_temp = GaleShapley(menRanking, womenRanking, logFile);  // Get the temporary matching
                            temporaryWoman = temporary_matches_temp[0][man];  
                            auto new_manipulatedPosition = find(it->second.begin(), it->second.end(), temporaryWoman);
                            if(manipulatedPosition > new_manipulatedPosition){
                                
                                logFile<<"Success! with difference "<< manipulatedPosition- new_manipulatedPosition<<" New ranking for the woman "<<woman<<" \n";
                                for (int i = 0; i < userRanking.size(); ++i) {
                                    logFile << userRanking[i] << " ";
                                }
                                logFile<<"\nOld manipulated ranking was \n";
                                for (int i = 0; i < userRanking.size(); ++i) {
                                    logFile << temporaryRanking[i] << " ";
                                }
                                logFile<<"Original matches \n";
                                print_matches(original_matches,logFile);
                                logFile<<"\nManipulation 1 matches\n";
                                print_matches(temporary_matches,logFile);
                                logFile<<"New matches ("<<man<<") should be getting the same match or better\n";
                                print_matches(temporary_matches_temp, logFile);

                                break;
                            }
                        }
                    }
                womenRanking[woman] = initialRanking;
            }
        }
    }
    //logFile<<"cannot make man better\n";
    womenRanking[woman] = initialRanking;
    
        }
    }
}


int main(int argc, char* argv[]) {
    
    if (argc == 2){
        n = atoi(argv[1]);
        cout<<"using "<<n<<" men\n";
        cout.flush();

    }
    ofstream trialFile("profiles.txt");
     ofstream trial1File("trial1File.txt");
       
      int  trials =20;
    for (int i = 0; i < n; i++) {
        men.push_back(to_string(i + 1));  
        women.push_back(string(1, 'A' + i)); 
    }


    for(int i=0; i<trials;i++){
        vector<map<string, vector<string> > > m = generateNamesAndPreferences(true,trialFile);
        Trial1(m[0],m[1],trial1File);
       
    }

   

    return 0;
}
