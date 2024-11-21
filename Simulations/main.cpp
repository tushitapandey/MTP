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

int n =15;
int fac(int n){
    if(n==0)
        return 1;
    if (n==1)
        return 1;
    return n* fac(n-1);
}
int total = fac(n) * fac(n) * n *n;         
vector<string> men;
vector<string> women;
bool print = true;
int totalIdealMatches = 0;
int totalCanImprove =0;



int global_best_matched=0;
int global_trivial_manipulations =0;
int global_successful_trivial_manipulations =0; 
int global_possible_non_trivial_manipulations =0;
int global_successful_non_trivial_manipulations=0;

bool detailed_log = false;
bool isNumeric(const string& str) {
    for (char ch : str) {
        if (!isdigit(ch)) {
            return false; // Return false if any character is not a digit
        }
    }
    return !str.empty(); // Return true only if all characters are digits and the string is not empty
}
bool isInSet(const string& value, const set<string>& validSet) {
    return validSet.find(value) != validSet.end();
}
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

int check(map<string, vector<string>>&  menRanking, 
            map<string, vector<string>>& womenRanking, 
             vector<map<string, string> >&  original_matches, 
             vector<map<string, string> >&  temporary_matches, 
             string& og_woman,
             string& og_woman_2,
             ofstream& logFile)
{
            string ogMan = original_matches[1][og_woman];  
            string newMan = temporary_matches[1][og_woman]; 
            auto originalPosition = find(womenRanking[og_woman].begin(), womenRanking[og_woman].end(), ogMan);
            auto manipulatedPosition = find(womenRanking[og_woman].begin(), womenRanking[og_woman].end(), newMan);
            auto originalIndex = distance(womenRanking[og_woman].begin(), originalPosition);
            auto manipulatedIndex = distance(womenRanking[og_woman].begin(), manipulatedPosition);
            int w2_diff = manipulatedIndex - originalIndex;

            ogMan = original_matches[1][ og_woman_2];  
            newMan = temporary_matches[1][ og_woman_2]; 
            originalPosition = find(womenRanking[ og_woman_2].begin(), womenRanking[ og_woman_2].end(), ogMan);
            manipulatedPosition = find(womenRanking[ og_woman_2].begin(), womenRanking[ og_woman_2].end(), newMan);
            originalIndex = distance(womenRanking[ og_woman_2].begin(), originalPosition);
            manipulatedIndex = distance(womenRanking[ og_woman_2].begin(), manipulatedPosition);
            int w1_diff = manipulatedIndex - originalIndex;

            if(w2_diff>0 && w1_diff >0 ) // both of the lying women gets strictly worse off  
                    return -1;
            // if(w2_diff>0 && w1_diff >0 ) // one of the lying women gets strictly worse off  
            //         return -1;
            for(auto rman : men){
                auto iter = menRanking.find(rman);
                string originalWoman = original_matches[0][rman];  
                string temporaryWoman = temporary_matches[0][rman];  
                originalPosition = find(iter->second.begin(), iter->second.end(),originalWoman);
                manipulatedPosition = find(iter->second.begin(), iter->second.end(), temporaryWoman);
                originalIndex = distance(iter->second.begin(), originalPosition);
                manipulatedIndex = distance(iter->second.begin(), manipulatedPosition);
                int man_diff = manipulatedIndex - originalIndex;
                if(man_diff <0){
                    // if(w1_diff<=0 && w2_diff<=0){
                    //     //logFile<<"Woman can make other woman better off while also improoving atleast 1 man"<<"\n";
                    //     //logFile<< rman <<" man " <<" difference is " << man_diff<<"\n";
                    //     //logFile<< "For woman "<<og_woman<<" , difference is "<< w1_diff<<"\n";
                    //     //logFile<< "For woman "<<og_woman_2<<" , difference is "<< w2_diff<<"\n";
                    // }
                    
                    if(w1_diff<0 || w2_diff <0){
                        logFile<<"It IS possible that a woman who is lying gets strictly better off even"
                                       <<" when there is a man who is strictly better off ";
                        logFile<< rman <<" man " <<" difference is " << man_diff<<"\n";
                        logFile<< "For woman "<<og_woman<<" , difference is "<< w1_diff<<"\n";
                        logFile<< "For woman "<<og_woman_2<<" , difference is "<< w2_diff<<"\n";
                        cout<<"It IS possible that a woman who is lying gets strictly better off even"
                                       <<" when there is a man who is strictly better off ";
                        cout<< rman <<" man " <<" difference is " << man_diff<<"\n";
                        cout<< "For woman "<<og_woman<<" , difference is "<< w1_diff<<"\n";
                        cout<< "For woman "<<og_woman_2<<" , difference is "<< w2_diff<<"\n";
                        cout.flush();

                    }
                    
                    return 1;
                }
            }
            
            return  -1;

}
//Is it possible for two women and a man to get better at the same time if the women are only performing push down manipulation => NO
//Can woman make other woman better off while also improoving atleast 1 man
void Trial1(map<string, vector<string>>  menRanking, map<string, vector<string>> womenRanking,ofstream& logFile){
    // string man = men[0];
    // string woman = women[0];
    vector<map<string, string> > original_matches= GaleShapley(menRanking, womenRanking, logFile);
    vector<map<string, string> > temp_matches;//= GaleShapley(menRanking, womenRanking, logFile);
    vector<string> original_pref_w1;
    vector<string> original_pref_w2;
    vector<string> preferences;
    vector<string> preferences_2; 
    for (int i = 0; i < n; i++) {
        preferences.push_back(to_string(i + 1));  
        preferences_2.push_back(to_string(i + 1));  
    }
    set<string> validSet = {"A", "B", "C", "D", "E","F"};
    int l=0;

    for(int i=0;i<n;i++){
        string woman_1 = women[i];
        original_pref_w1 = womenRanking[woman_1];
        
        do {
            womenRanking[woman_1] = preferences;
            
            if(womenRanking[woman_1].size()!=n){
                for(auto m: womenRanking[woman_1]){
                    logFile<<m<<" ";
                }
                logFile<<"\ndebug 1";
                return;
            }
            for(int j=i+1;j<n;j++){
                string woman_2 = women[j];
                original_pref_w2 = womenRanking[woman_2];
                do {
                    if(womenRanking[woman_2].size()!=n){
                        for(auto m: womenRanking[woman_2]){
                            logFile<<m<<" ";
                        }
                        logFile<<"\ndebug 2";
                        return;
                    }
                    temp_matches = GaleShapley(menRanking, womenRanking, logFile);
                    womenRanking[woman_1] = original_pref_w1;
                    womenRanking[woman_2] = original_pref_w2;
                    cout << "\r" << 200.0 * (l++) / total << "% done ";
                    cout.flush();

                     if (isNumeric(woman_2) || !isInSet(woman_2, validSet)) {
                        logFile<< woman_2 << " is wrong\n";
                        logFile.flush();
                        return;
                    } 
                    // logFile<<"Women: "<<woman_1<<" "<<woman_2<<"\n";
                    logFile.flush();
                    if(check(menRanking,womenRanking,original_matches,temp_matches,woman_1, woman_2,logFile)==1){

                        for (auto& man : men) {
                                logFile << man << " : ";
                                for (auto& w :menRanking[man])
                                    logFile << w << " ";
                                logFile << "\n";
                            }
                        for (auto& wman : women) {
                                logFile << wman << " : ";
                                for (auto& m :womenRanking[wman])
                                    logFile << m << " ";
                                logFile << "\n";
                            }
                        logFile<<"Original\n";
                        print_matches(original_matches,logFile);
                        logFile<<"Changed\n";
                        print_matches(temp_matches,logFile);
                        logFile<<woman_1<<" : ";
                        for(auto item: preferences)
                            logFile<<item<<" ";
                        logFile<<"\n"<<woman_2<<" : ";
                        for(auto item: preferences_2)
                            logFile<<item<<" ";
                            logFile.flush();
                        return;
                    }
                }while (next_permutation(preferences_2.begin(), preferences_2.end()));
            }
        } while (next_permutation(preferences.begin(), preferences.end()));
    }
    cout<<l<<"\n";
}

bool betterWoman(map<string, vector<string>> menRanking,
                map<string, vector<string>> womenRanking , 
                string man, 
                string woman,
                ofstream& logFile, 
                vector<map<string, string> > original_matches){
    vector<map<string, string> > temporary_matches;
    vector<string> initialRanking = womenRanking[woman];
    auto iterator= find(womenRanking[woman].begin(), womenRanking[woman].end(), original_matches[1][woman]);
    //int swapRanking =  distance(womenRanking[woman].begin(), iterator) ;
    int swapRanking =1;
    vector<string> temporaryRanking = initialRanking;
    string swapMan = temporaryRanking[swapRanking];
    temporaryRanking.erase(temporaryRanking.begin() + swapRanking++);
    temporaryRanking.insert(temporaryRanking.begin(), swapMan);
    womenRanking[woman] = temporaryRanking;  
    temporary_matches = GaleShapley(menRanking, womenRanking, logFile);  // Get the temporary matching
    string temporaryWoman = temporary_matches[0][man];  
   auto it = menRanking.find(man);
    auto originallyGoodPos = find(it->second.begin(), it->second.end(), woman);
    auto manipulatedPosition = find(it->second.begin(), it->second.end(), temporaryWoman);
    int originalIndex = distance(it->second.begin(), originallyGoodPos);
    int manipulatedIndex = distance(it->second.begin(), manipulatedPosition);
    if (manipulatedIndex < originalIndex) {
        //logFile<<"better to "<<temporaryWoman;
        // betterOffCount++;  
        //  menStrictlyBetterOffByBetterWoman ++;
        womenRanking[woman] = initialRanking;
        return true;
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
            if (manipulatedIndex < originalIndex) {
                //logFile<<"better to "<<temporaryWoman;
                //  menStrictlyBetterOffByBetterWoman ++;
                // betterOffCount++;  
                womenRanking[woman] = initialRanking;
               return true;
            }
        }
    }
    //logFile<<"cannot make man better\n";
    womenRanking[woman] = initialRanking;
    return false;
}


void Trial2(map<string, vector<string>>  menRanking, map<string, vector<string>> womenRanking,ofstream& logFile){
    vector<map<string, string> > original_matches ;
    original_matches= GaleShapley(menRanking, womenRanking, logFile);
    vector<map<string, string> > temporary_matches;
     int countIdealMatches = 0; 
    int countCanImprove = 0; 
    map<string, int> improvementPossible;  
    if(detailed_log)        
        print_matches(original_matches,logFile);
    for(string man: men){
        improvementPossible[man] = 0;
        int trivial_match =0;
        if(detailed_log)        
            logFile<<"\nMan "<<man;
        int betterOffCount = 0;
        if(original_matches[0][man]==  menRanking[man][0]){
            if(detailed_log)        
                logFile<<" already best matched\n";
            countIdealMatches++;
            continue;
        }
        int currentMatchIndex = find(menRanking[man].begin(), menRanking[man].end(), original_matches[0][man]) - menRanking[man].begin();
        for(string woman: menRanking[man]){
            int womanIndex = find(menRanking[man].begin(), menRanking[man].end(), woman) - menRanking[man].begin();
            if (womanIndex < currentMatchIndex) {
                if(detailed_log)        
                {
                    //logFile<<woman<<": can't manipulate woman higher in the ranking list\n";
                    logFile<<"Trivial manipulation "<<woman<<" ";
                    // betterWoman(menRanking,womenRanking,man,woman,logFile,original_matches);
                }
                improvementPossible[man] ++;
                trivial_match++;

                continue;
            }
            vector<string> initialRanking = womenRanking[woman];
            auto iterator= find(womenRanking[woman].begin(), womenRanking[woman].end(), original_matches[1][woman]);
            int swapRanking =  distance(womenRanking[woman].begin(), iterator) +1;
            // int swapRanking =1;
            if(swapRanking == womenRanking[woman].size())   
                continue;
            vector<string> temporaryRanking = initialRanking;
            string swapMan = temporaryRanking[swapRanking];
            temporaryRanking.erase(temporaryRanking.begin() + swapRanking++);
            temporaryRanking.insert(temporaryRanking.begin(), swapMan);
            womenRanking[woman] = temporaryRanking;  
            temporary_matches = GaleShapley(menRanking, womenRanking, logFile);  // Get the temporary matching
            string originalWoman = original_matches[0][man];  
            string temporaryWoman = temporary_matches[0][man];  
            auto it = menRanking.find(man);
            auto originalPosition = find(it->second.begin(), it->second.end(), originalWoman);
            auto manipulatedPosition = find(it->second.begin(), it->second.end(), temporaryWoman);
            int originalIndex = distance(it->second.begin(), originalPosition);
            int manipulatedIndex = distance(it->second.begin(), manipulatedPosition);
            if (manipulatedIndex < originalIndex) {
                betterOffCount++;  
                improvementPossible[man] ++;
                break;
            }else{
                for(; swapRanking< temporaryRanking.size();swapRanking++){
                    iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swapRanking);
                    womenRanking[woman] = temporaryRanking;
                    temporary_matches = GaleShapley(menRanking, womenRanking, logFile);
                    originalWoman = original_matches[0][man];  
                    temporaryWoman = temporary_matches[0][man];  
                    originalPosition = find(it->second.begin(), it->second.end(), originalWoman);
                    manipulatedPosition = find(it->second.begin(), it->second.end(), temporaryWoman);
                    originalIndex = distance(it->second.begin(), originalPosition);
                    manipulatedIndex = distance(it->second.begin(), manipulatedPosition);
                    if (manipulatedIndex < originalIndex) {
                        improvementPossible[man] ++;
                        break;
                    }
                }
            }
            womenRanking[woman] = initialRanking;
        }
        if(detailed_log){
            logFile<<"\n";
            logFile<<" Women he can manipulate to get better "<< improvementPossible[man] ;
            logFile<<" \nNon trivial women he can manipulate to get better "<< improvementPossible[man]-trivial_match ;//<<" women to make himself strictly better";
    

        }
        
    }
    
    
    logFile<< "Metric 1: " << countIdealMatches << " men matched with their most ideal partner.\n";
    // logFile << "Metric 2:\n";
    for (const auto& entry : improvementPossible) {
         if(detailed_log)
            logFile<< "Man " << entry.first << " can get better off with " << entry.second << " women manipulating their rankings.\n";
        if(entry.second >0)
            countCanImprove++;

    }
    logFile<< "Metric 2: " << countCanImprove << " can get better off by manipulating a woman.\n";
    totalCanImprove+= countCanImprove;
    totalIdealMatches += countIdealMatches;
}


void Trial3(map<string, vector<string>>  menRanking, map<string, vector<string>> womenRanking,ofstream& logFile){
    vector<map<string, string> > original_matches ;
    original_matches= GaleShapley(menRanking, womenRanking, logFile);
    vector<map<string, string> > temporary_matches;
    if(detailed_log)
        print_matches(original_matches,logFile);
    int allMen_best_matched=0;
    int allMen_trivial_manipulations =0;
    int allMen_successful_trivial_manipulations =0;
    int allMen_possible_non_trivial_manipulations =0;
    int allMen_successful_non_trivial_manipulations=0;
    for(string man: men){
        int best_matched = 0 ;
        int trivial_manipulations =0;
        int possible_non_trivial_manipulations =0;
        int successful_non_trivial_manipulations=0;
        int successful_trivial_manipulations=0;
        if(detailed_log){
            logFile<<"Man "<<man;

        }
        
        if(original_matches[0][man]==  menRanking[man][0]){
            best_matched+=n; // no match needed
            allMen_best_matched+= best_matched;
            if(detailed_log){
                        logFile<<" already best matched\n";
                    }
            
            continue;
        }
        if(detailed_log)
                logFile<<"\n";
        int currentMatchIndex = find(menRanking[man].begin(), menRanking[man].end(), original_matches[0][man]) - menRanking[man].begin();
        for(string woman: menRanking[man]){
            int womanIndex = find(menRanking[man].begin(), menRanking[man].end(), woman) - menRanking[man].begin();
            if(detailed_log)
                logFile<<" Woman"<<woman<<" ";
            if (womanIndex < currentMatchIndex) {
                trivial_manipulations++;
                if(detailed_log)
                    logFile<<"trivial manipulator\n";
                if(betterWoman(menRanking,womenRanking,man,woman,logFile,original_matches)){
                    successful_trivial_manipulations++;
                }
                continue;
            }
            if(detailed_log)
                logFile<<"\n";

            possible_non_trivial_manipulations++;
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
            auto originalPosition = find(it->second.begin(), it->second.end(), originalWoman);
            auto manipulatedPosition = find(it->second.begin(), it->second.end(), temporaryWoman);
            int originalIndex = distance(it->second.begin(), originalPosition);
            int manipulatedIndex = distance(it->second.begin(), manipulatedPosition);
            int man_diff =manipulatedIndex -  originalIndex ;
            if(detailed_log){
                        logFile<<"  "<<man_diff<<" with"; 
                        for(auto m:  womenRanking[woman] )
                            logFile<<" "<<m;
                        logFile<<"\n";

                    }
            if (manipulatedIndex < originalIndex) {
                successful_non_trivial_manipulations++;
                break;
            }else{
                for(; swapRanking< temporaryRanking.size();swapRanking++){
                    iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swapRanking);
                    womenRanking[woman] = temporaryRanking;
                    temporary_matches = GaleShapley(menRanking, womenRanking, logFile);
                    originalWoman = original_matches[0][man];  
                    temporaryWoman = temporary_matches[0][man];  
                    originalPosition = find(it->second.begin(), it->second.end(), originalWoman);
                    manipulatedPosition = find(it->second.begin(), it->second.end(), temporaryWoman);
                    originalIndex = distance(it->second.begin(), originalPosition);
                    manipulatedIndex = distance(it->second.begin(), manipulatedPosition);
                    man_diff = manipulatedIndex -  originalIndex ;
                    if(detailed_log){
                        logFile<<"  "<<man_diff<<" with"; 
                        for(auto m:  womenRanking[woman] )
                            logFile<<" "<<m;
                        logFile<<"\n";

                    }
                    
                    if (manipulatedIndex < originalIndex) {
                        successful_non_trivial_manipulations++;
                        break;
                    }
                    
                }
            }
            womenRanking[woman] = initialRanking;
            
            

        }
        if(detailed_log){
            logFile<<" Trivial Manipulations "<<trivial_manipulations<<"\n";
            logFile<<" Possible non trivial manipulations "<<possible_non_trivial_manipulations<<"\n";
            logFile<<" Successful non trivial manipulations "<<successful_non_trivial_manipulations<<"\n";
        }
        
            
        
        allMen_trivial_manipulations+= trivial_manipulations;
        allMen_possible_non_trivial_manipulations+= possible_non_trivial_manipulations;
        allMen_successful_non_trivial_manipulations+= successful_non_trivial_manipulations;
        allMen_successful_trivial_manipulations+= successful_trivial_manipulations;
    }
    logFile<<"Best Matched pairs  "<<allMen_best_matched<<"\n";
    logFile<<"Trivial Manipulations "<<allMen_trivial_manipulations<<"\n";
    logFile<<"Possible non trivial manipulations "<<allMen_possible_non_trivial_manipulations<<"\n";
    logFile<<"Successful non trivial manipulations "<<allMen_successful_non_trivial_manipulations<<"\n";

    global_best_matched+= allMen_best_matched;
    global_trivial_manipulations+=allMen_trivial_manipulations;
    global_possible_non_trivial_manipulations+=allMen_possible_non_trivial_manipulations;
    global_successful_non_trivial_manipulations+=allMen_successful_non_trivial_manipulations;
    global_successful_trivial_manipulations += allMen_successful_trivial_manipulations;



    

    
}

void customPreferences(ofstream& logFile){
    ifstream inputFile("preferences.txt");
    string line;
    map<string, vector<string>> menRanking;
    map<string, vector<string>> womenRanking;
    if (!inputFile) {
        cerr << "Error: Could not open file." << endl;
        return ;
    }

    while (getline(inputFile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string key, value;
        vector<string> preferences;
        ss >> key;
        if (key.back() == ':') key.pop_back();
        while (ss >> value) {
            if (value.back() == ':') value.pop_back();
            preferences.push_back(value);
            
        }
        if (isdigit(key[0])) {
            menRanking[key] = preferences;
        } else if (isalpha(key[0])) {
            womenRanking[key] = preferences;
        }
    }
    

    vector<map<string, string> > matches = GaleShapley(menRanking,womenRanking,logFile);
    print_matches(matches,logFile);
    womenRanking["A"] = {"3", "4", "5", "1", "2", "6"};
    womenRanking["B"] = {"1", "2", "3", "4", "5", "6"};
    matches = GaleShapley(menRanking,womenRanking,logFile);
    print_matches(matches,logFile);

    

}


int main(int argc, char* argv[]) {
    if (argc == 2)
        n = atoi(argv[1]);
    for (int i = 0; i < n; i++) {
        men.push_back(to_string(i + 1));  
        women.push_back(string(1, 'A' + i)); 
    }
    int trials =1000;
    bool trial_1=false, trial_2=false,trial_3=false;
    // ofstream resultFile("results.txt");
    ofstream finalResultFile("15Women_results.txt");
    ofstream trial1File("trial_1.txt");
    ofstream trial2File("trial_2.txt");
    ofstream trial3File("trial_3.txt");
    trial_3=true;
    // trial_2=true;
    for(int i=0; i<trials;i++){
        vector<map<string, vector<string> > > m = generateNamesAndPreferences(false,finalResultFile);
        if(trial_1){
            trial1File<<"Trial #"<<i<<"\n";
            Trial1(m[0],m[1],trial1File);
        }
        if(trial_2){
            trial2File<<"Trial #"<<i<<"\n";
            Trial2(m[0],m[1],trial2File);
        }
        if(trial_3){
            trial3File<<"Trial #"<<i<<"\n";
            Trial3(m[0],m[1],trial3File);
        }
    }
     finalResultFile << "With "<<n<<" men/women, After " << trials  <<" trials:\n\n";
    if(trial_1){
       finalResultFile<<"Trial 1 was investigative only to check existence, check logs\n";
    }
    if(trial_2){
       finalResultFile<<"Trial 2\n";
        finalResultFile << "Total Metric 1: " << 100.0*totalIdealMatches/(n*trials) << "% men matched with their most ideal partner.\n";
        // finalResultFile << "Total Metric 2: " << 100.0*totalIdealMatches/(n*trials) << "% men can improve by manipulating a woman above their GS match \n";
        // finalResultFile << "Total Metric 2: " << 100.0*totalCannotImprove/(n*trials) << "% men cannot improve by manipulating a woman lower in their ranking than the match they got.\n";
        finalResultFile << "Total Metric 2: " << 100.0*totalCanImprove/(n*trials) << "% men can improve by manipulating a woman \n\n";
        // finalResultFile << "Total Metric 4: " << 100.0*menStrictlyBetterOffByBetterWoman /(n*trials) << "% men can improve to a woman higher than the lying woman in their ranking.\n";
    }
    if(trial_3){
    //    finalResultFile<<"Trial 3\n";
        finalResultFile<<"Best Matched pairs "<<100.0*global_best_matched/(n*n*trials)<<"\n";
        finalResultFile<<"Total Trivial Manipulations "<<100.0*global_trivial_manipulations/(n*n*trials)<<"\n";
        
        
        finalResultFile<<"Possible non trivial manipulations "<<100.0*global_possible_non_trivial_manipulations/(n*n*trials)<<"\n";
        
        finalResultFile<<"Successful Trivial Manipulations"<< 100.0*global_successful_trivial_manipulations/(n*n*trials)<<"\n";
        finalResultFile<<"Successful non trivial manipulations "<<100.0*global_successful_non_trivial_manipulations/(n*n*trials)<<"\n";

        // finalResultFile<<"Successful non trivial manipulations as a percentage of possible trivial manipulations "<<100.0*global_successful_non_trivial_manipulations/(global_non_trivial_manipulations)<<"\n";
        // finalResultFile<<"Trivial Manipulations in which lying woman takes man to a woman better than her as a pentage of total trivial maipulation "<< 100.0*global_successful_trivial_manipulations/(n*n*trials)<<"\n";

        cout<<"Sanity check " <<global_best_matched+global_trivial_manipulations+global_possible_non_trivial_manipulations - n*n*trials<<"\n";
    }

    return 0;
}
