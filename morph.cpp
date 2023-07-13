#include <vector>
#include <string>
#include <iostream>
#include "words.hpp"

using namespace std;

class word
{
private:
    int num_affixes;
    vector<string> affixes;
    string root_word;
    vector<string> power_set; // might or might not be useful
    vector<string> possible_stems;
    /* data */

public:
    word(string word) : root_word(word){};
    bool is_verb(string word);
    void break_stem_by_suffix(string _current_stem);
    void break_stem_by_prefix(string _current_stem);
    void break_by_adjective(string _current_stem);
    bool is_noun(string word);
    int starts_with_affix(string word);
    bool is_vowel(char c);
    bool is_conjugated_verb(string word);

    void set_affixes()
    {
        cout << "Root word: " << root_word << endl;
        // First possibility, the word is a verb
        if (this->is_verb(root_word))
        {
            // TODO: Handle adverbs
            cout << "Verb" << endl;
            // Extract the suffix, which is usually the last character
            string suffix = root_word.substr(root_word.length() - 1, 1);
            // Put the sufix and prefix in the affixes vector after converting suffix to string
            affixes.push_back(suffix);
            // push the first two characters into affixes
            string prefix = root_word.substr(0, 2);
            affixes.push_back(prefix);
            

            // We are now left with a verb that might have extra prefixes (i.e: Kw-iyegerez-a; which comes from kw-eger-a)
            // We need to check if the verb has extra prefixes
            // One possible way to extract more prefixes is in the case of kw- vervs; where the first word after kw- will also be a prefix
            // if(root_word.substr(0,2) == "kw"){
            //     affixes.push_back(root_word.substr(2,1));
            // }

            // What we get here is one possible stem; we can deduce mini-stems that come from this word( kw-i-yegerez-a -> kw-i-eger-ez-a))
            // We need a list of these little things that come at the end of a verb(similar to ez or ij)        }
            string current_stem = root_word.substr(2, root_word.length() - 3);
            
            possible_stems.push_back(current_stem);
            // Also push the stem that contains the last letter
            string last_stem = root_word.substr(2, root_word.length() - 2);
            possible_stems.push_back(last_stem);

            // Get more stems
            break_stem_by_suffix(current_stem); // We break the stem by suffixes first, getting more possible stems and more affixes
            break_stem_by_prefix(current_stem); // same thing but with prefixes
            break_by_adjective(current_stem);   // same thing but with adjectives
        }
        else if (this->is_conjugated_verb(root_word))
        {
            cout << "Conjugated verb" << endl;
            // Extract the suffix, which is usually the last character
            string suffix = root_word.substr(root_word.length() - 1, 1);
            affixes.push_back(suffix);
            // push the characters between 0 and starts_with_affix into affixes
            string prefix = root_word.substr(0, starts_with_affix(root_word));
            affixes.push_back(prefix);
            
            possible_stems.push_back(root_word.substr(starts_with_affix(root_word), root_word.length() - starts_with_affix(root_word) - 1));

            string current_stem = root_word.substr(starts_with_affix(root_word), root_word.length() - starts_with_affix(root_word) - 1);
            // cout<< "Current stem: " << current_stem << endl;
            break_stem_by_suffix(current_stem); // We break the stem by suffixes first, getting more possible stems and more affixes
            break_stem_by_prefix(current_stem); // same thing but with prefixes
            break_by_adjective(current_stem);   // same thing but with adjectives
        }
        // Second possibility, the word is a noun
        else if (this->is_noun(root_word))
        {
            possible_stems.push_back(root_word);
            // This is very complicated, and should be done case by case
            cout << "Noun" << endl;
            // We already have a stem here, time to put it in our vector
            int start = 0; // In case we get something like mugabo instead of umugabo
            if (this->is_vowel(root_word[0]))
            {
                start = 1;
                affixes.push_back(root_word.substr(0, 1));
                possible_stems.push_back(root_word.substr(1, root_word.length() - 1));
            }
            // We need to check if the noun has extra prefixes, especially the -mu -ba -mu -mi -ri -ma types
            for (int i = 0; i < kirundi_affixes.size(); i++)
            {
                if (root_word.substr(start, kirundi_affixes[i].length()) == kirundi_affixes[i])
                {
                    affixes.push_back(kirundi_affixes[i]);
                    possible_stems.push_back(root_word.substr(kirundi_affixes[i].length() + start, root_word.length() - kirundi_affixes[i].length() - start)); // inka -> i-n-[ka].push-back
                    break;                                                                                                                                     // For now, let's only work with one affix
                }
            }
            // check if there are some adverbs
            break_by_adjective(possible_stems[possible_stems.size() - 1]);
        }
        else if(false){
            // TODO: Adjectives
        } else if(false){
            // TODO: Negation
        } else if(false){
            // TODO: True adverbs (like, real ones not the ones I am calling `adverbs`)
        } else if(false){
            // TODO: Other types I might not have accounted for
        }
    }
    void print_all_affixes_and_all_stems()
    {
        cout << "Affixes: " << affixes.size() << " " << endl;
        for (int i = 0; i < affixes.size(); i++)
        {
            cout << affixes[i] << endl;
        }
        cout << "Stems: " << possible_stems.size() << " " << endl;
        for (int i = 0; i < possible_stems.size(); i++)
        {
            cout << possible_stems[i] << endl;
        }
    }
};

bool word::is_noun(string word)
{
    char first_char = word[0];
    if (first_char == 'a' || first_char == 'u' || first_char == 'i')
    { // Careful as "ku" is also for verbs
        // The first character is always a, i, or u for nouns
        // Followed by one of the -mu -ba -mu -mi -ri -ma or not (i.e: ivi ici isi)
        // Exceptions exist: iyo ico abo aba uwa uwi uti ubu, etc (we will hopefully handle these later)
        return true;
    }
    else
    {
        return false;
    }
}

bool word::is_verb(string word)
{
    // if the first two letters are "ku" or "kw" then it is a verb
    // TODO: Handle uppercase
    if (((word[0] == 'k' && word[1] == 'u') || (word[0] == 'k' && word[1] == 'w') || (word[0] == 'g' && word[1] == 'u')) && (word.length() > 2) && (word[word.length() - 1] == 'a' || word[word.length() - 1] == 'e'))
    {
        return true;
    }
    else
    {
        return false;
    }
}

int word::starts_with_affix(string s)
{
    for (int i = 0; i < kirundi_affixes.size(); i++)
    {
        if (s.substr(0, kirundi_affixes[i].length()) == kirundi_affixes[i])
        {
            return kirundi_affixes[i].length();
        }
    }
    return 0;
}

bool word::is_vowel(char n)
{
    if (n == 'a' || n == 'e' || n == 'i' || n == 'o' || n == 'u')
    {
        return true;
    }
    else
    {
        return false;
    }
}

void word::break_stem_by_suffix(string _current_stem)
{
    // Recursively call break_stem_by_suffix on our current stem to check if it ends with any of the prefixes in words.hpp kirundi_verb_suffixes vector
    for (int i = 0; i < kirundi_verb_suffixes.size(); i++)
    {
        // If the current stem ends with a prefix, then we need to break it down further
        int len = _current_stem.length() - kirundi_verb_suffixes[i].length();
        if (len < 1)
        {
            continue;
        }
        if (_current_stem.substr(len, _current_stem.length() - 1) == kirundi_verb_suffixes[i])
        {
            // Push the prefix into affixes
            affixes.push_back(kirundi_verb_suffixes[i]);
            // Push the stem into possible_stems
            possible_stems.push_back(_current_stem.substr(0, len));
            // Call break_stem_by_suffix on the new stem
            break_stem_by_suffix(_current_stem.substr(0, len));
            break;
        }
    }
}

void word::break_stem_by_prefix(string _current_stem)
{
    if (_current_stem.length() < 2)
    {
        return;
    }
    // do the same as break_stem_by_suffix but with kirundi_affixes instead
    for (int i = 0; i < kirundi_affixes.size(); i++)
    {
        // If the current stem starts with a prefix, then we need to break it down further
        int len = kirundi_affixes[i].length() - _current_stem.length();
        if (-len < 1)
        {
            continue;
        }
        if (_current_stem.substr(0, kirundi_affixes[i].length()) == kirundi_affixes[i])
        {
            // Push the prefix into affixes
            affixes.push_back(kirundi_affixes[i]);
            // Push the stem into possible_stems
            possible_stems.push_back(_current_stem.substr(kirundi_affixes[i].length(), _current_stem.length() - 1));
            // Call break_stem_by_suffix on the new stem
            break_stem_by_prefix(_current_stem.substr(kirundi_affixes[i].length(), _current_stem.length() - 1));
            break;
        }
    }
}

bool word::is_conjugated_verb(string word)
{
    // The rules are as follows: First comes the pronoun: -mu -ba -mu -mi -ri -ma; then an adverb, then the stem, then multiple suffixes
    // Eg: ndagiye-> n-da-giy-e (later I will implement a way to break giy-> gend- ye); or ndageze ->n-da-gir-ye
    bool found_pronoun = false;
    int curr_idx = this->starts_with_affix(word);
    if(curr_idx){
        found_pronoun = true;
    } 
    if (found_pronoun)
    {
        // if the first two chars after i are in kirundi_adverbs, return true
        // if (word.length() < 2)
        // {
        //     return false;
        // }
        // int k = 0;
        // for (int j = 0; j < kirundi_adverbs.size(); j++)
        // {
        //     int len = word.length() - kirundi_adverbs[j].length();
        //     if (len < 1)
        //     {
        //         continue;
        //     }
        //     if (word.substr(curr_idx, kirundi_adverbs[j].length()) == kirundi_adverbs[j])
        //     {
        //         // First adverb (there can be many)
        //         k += kirundi_adverbs[j].length();
        //         break;
        //     }
        // }
        // if(k != 0) return true; // this is not necessary
        return true;
        
    }
    // and we
    return false;

    // We have now either a pure stem, or a stem with multiple prefixes; gotta change that: i.e: tu-ra-bibabwiy-e -> tu-ra-bi-ba-bwiy-e
    // Oh, yeah, we have a function that just does that!
}

void word::break_by_adjective(string word)
{
    // Now we find the adverb(s)
    if (word.length() < 2)
    {
        return;
    }
    for (int i = 0; i < kirundi_adverbs.size(); i++)
    {
        int len = word.length() - kirundi_adverbs[i].length();
        if (len < 1)
        {
            continue;
        }
        if (word.substr(0, kirundi_adverbs[i].length()) == kirundi_adverbs[i])
        {
            // First adverb (there can be many)
            affixes.push_back(kirundi_adverbs[i]);
            possible_stems.push_back(word.substr(kirundi_adverbs[i].length(), word.length() - 1));
            break_by_adjective(word.substr(kirundi_adverbs[i].length(), word.length() - 1));
            break; 
        }
    }
}
int main()
{
    word test("bagarika");
    // test.break_by_adjective("ranatangara");
    test.set_affixes();
    test.print_all_affixes_and_all_stems();
    return 0;
}