#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <math.h>
#include <algorithm>
#include<time.h>

using namespace std;

float cond_prob(string a, char b, int cnt_of_a, string src) //P("b"|"a")
{
    string str_to_find = a+b;
    int cnt = 0, i=0;

    while(i <= src.length()-str_to_find.length())
    {
        if(src.substr(i,str_to_find.length()) == str_to_find)
            cnt++;
        i++;
    }

    return (float)cnt/(float)cnt_of_a;
}

int cnt_a(string a, string src)
{
    int cnt=0, i=0;
    while(i <= src.length()-a.length()-1)
    {
        if(src.substr(i,a.length()) == a)
            cnt++;
        i++;
    }
    return cnt;
}

string next_char(string start, string src, vector<char>& alph, int order)
{
    map<char, float> probs;
    string a = start.substr(start.length()-order, order);
    int cnt_of_a = cnt_a(a, src);
    for(char el : alph)
        probs.insert({el, cond_prob(a, el, cnt_of_a, src)});

    double rndNumber = rand() / (double) RAND_MAX;
    double offset = 0.0;
    char pick;

    for (auto const& el : probs)
    {
        offset += el.second;
        if (rndNumber < offset)
        {
            pick = el.first;
            break;
        }
    }
    cout<<pick;
    return start+pick;
}

void markov(string start, string src, vector<char>& alph, int n, int order)
{
    cout<<start;
    while(start.length() < order)
        start = next_char(start, src, alph, start.length());

    for(int i=start.length()-1; i < n; i++)
        start = next_char(start, src, alph, order);
}

void print_probs_in_alph(string a, string src, vector<char>& alph)
{
    int cnt_of_a = cnt_a(a, src);
    float sum=0, p;
    for(char el : alph)
    {
        p = cond_prob(a, el, cnt_of_a, src);
        sum += p;
        cout<<"P('"<<a<<"|"<<el<<"') = "<<p<<endl;
    }
    cout<<"sum: "<<sum<<endl;
}

void print(vector<char>& available_letters, map<char, int>& letters_count)
{
    int r;
    while(available_letters.size() > 0)
    {
        r = rand() % available_letters.size();
        cout<<available_letters[r];
        letters_count[available_letters[r]]-=1;
        if(letters_count[available_letters[r]] == 0)
            available_letters.erase(remove(available_letters.begin(), available_letters.end(), available_letters[r]));
    }
}

void gen(ifstream& src, vector<char>& alph, int n)
{
    map<char, float> zero_prob;
    map<char, int> letter_ocur;
    map<char, float> first_prob;
    for(char letter : alph)
    {
        zero_prob.insert(pair<char, float>(letter, (float)1/alph.size()));
        letter_ocur.insert(pair<char, int>(letter, 0));
    }

    char let;
    int cnt=0;
    string text="";
    while(!src.eof())
    {
        src.get(let);
        letter_ocur[let]++;
        text += let;
        cnt++;
    }

    map<char, int> zero_let_map;
    map<char, int> first_let_map;
    int p;
    for(char letter : alph)
    {
        first_prob.insert(pair<char, float>(letter, (float)letter_ocur[letter]/cnt));
        p = round(n*zero_prob[letter]);
        if(p > 0)
            zero_let_map.insert(pair<char, int>(letter, p));
        p = round(n*first_prob[letter]);
        if(p > 0)
            first_let_map.insert(pair<char, int>(letter, p));
    }

    char most_freq, nd_most_freq;
    int maxi=0, nd_maxi=0;
    for(auto const& el : letter_ocur)
    {
        if(el.second > maxi)
        {
            nd_maxi = maxi;
            maxi = el.second;
            nd_most_freq = most_freq;
            most_freq = el.first;
        }
        else if(el.second > nd_maxi)
        {
            nd_maxi = el.second;
            nd_most_freq = el.first;
        }
    }

    vector<char> available_letters = alph;
    cout<<"Zero-order approximation: "<<endl;
    print(available_letters, zero_let_map);
    cout<<endl<<endl<<"First-order approximation: "<<endl;

    available_letters.clear();
    for(auto const& el : first_let_map)
        available_letters.push_back(el.first);
    print(available_letters, first_let_map);

    string most_fr = "";
    most_fr += most_freq;
    cout<<endl<<endl<<"For most frequent character: "<<most_fr<<endl;
    print_probs_in_alph(most_fr, text, alph);
    string nd_most_fr = "";
    nd_most_fr += nd_most_freq;
    cout<<endl<<endl<<"For second most frequest character: "<<nd_most_fr<<endl;
    print_probs_in_alph(""+nd_most_fr, text, alph);

    if(most_fr == " ")
        most_fr = nd_most_fr;


    cout<<endl<<"First-order Markov source: "<<endl;
    markov(most_fr, text, alph, n, 1);

    cout<<endl<<endl<<"Third-order Markov source: "<<endl;
    markov(most_fr, text, alph, n, 3);

    cout<<endl<<endl<<"Fifth-order Markov source: "<<endl;
    markov("probability", text, alph, n, 5);
}

int main()
{
    ifstream hamlet("norm_hamlet.txt");
    ifstream romeo("norm_romeo_and_juliet.txt");
    ifstream wiki("norm_wiki_sample.txt");

    if(hamlet.is_open() && romeo.is_open() && wiki.is_open())
    {
        vector<char> alph;
        alph.push_back(' ');
        for(int i=0; i < 26; i++)
            alph.push_back((char)('a'+i));
        for(int i=0; i < 10; i++)
            alph.push_back((char)('0'+i));

        srand(time(NULL));
        cout<<"HAMLET: "<<endl;
        gen(hamlet, alph, 100);

        cout<<endl<<endl<<"ROMEO AND JULIET: "<<endl;
        gen(romeo, alph, 100);

        cout<<endl<<endl<<"WIKI: "<<endl;
        gen(wiki, alph, 30);

        hamlet.close();
        romeo.close();
        wiki.close();
    }

    return 0;
}
