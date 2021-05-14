#include "SkipList.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

using namespace std;

SkipList::SkipList(char* virusName)
{
    /*Arxikopoihsh tou node sentinel*/
    sentinel = (Node*) malloc(sizeof(Node));
    sentinel->next = (Node**) malloc(sizeof(Node*));
    sentinel->next[0] = NULL;
    sentinel->height = 0;
    sentinel->record = NULL;
    this->numNodes = 0;
    this->virusName = virusName;
    
}


SkipList::~SkipList()
{
    Node* fr = sentinel;
    Node* head = sentinel;

    while (head != NULL)
    {
        head = head->next[0];   
        free (fr->next);
        free (fr);
        fr = head;
    }
}



void SkipList::Insert(Record* rec)
{
    string id_str = rec->get_id();
    int id = stoi (id_str);
    int i;
    Node** more_height;
    Node* head;
    bool coin_head;   
    srand (time(0));


    Node* node = (Node*) malloc (sizeof(Node));
    node->record = rec;
    node->next = (Node**) malloc(sizeof(Node*));
    node->height = 0;

    //insert the first node
    if (sentinel->next[0]== NULL && sentinel->height == 0) {     
        sentinel->next[0] = node;
        node->next[0] = NULL;
        numNodes++;
        return;
    }

    int before = log (numNodes);
    int after = log (++numNodes);
    bool heightInc = (before != after);  //increase the height of sentinel if it is true  

    
    if (heightInc && sentinel->height < MaxHeight)  
    {      
        sentinel->height++;
        more_height = (Node**) realloc (sentinel->next, (sentinel->height+1)*sizeof(Node*));

        if(more_height != NULL) {
            sentinel->next = more_height;
            sentinel->next[sentinel->height] = NULL;    //initialize the new height
        }
        else {
            puts("Error reallocating memory in sentinel (SkipList)");
            free(sentinel->next);
            exit(1);
        }

    }

    /* Avxanw to upsos tou node pou theloume na eisagoume tuxaia mexri to upsos tou sentinel*/
    //to 0 epipedo uparxei gia auto paei mexri to sentinel_height
    for (i = 1; i <= sentinel->height; i++) 
    {
        coin_head = (rand()%2 != 0);
        if (coin_head)
        {
            node->height++;
            more_height = (Node**)realloc(node->next, ((node->height+1)) * sizeof(Node*));
            if (more_height != NULL) {
                node->next = more_height;
                node->next[node->height] = NULL;
            }        
            else {
                puts("Error reallocating memory at node in Insert (SkipList)");
                free(sentinel->next);
                exit(1);
            }
        }
    }


    Node* prev_nodes[sentinel->height+1];  //gia na thn katagrafi twn prougoumenwn nodes
    int lv = sentinel->height;
    head = sentinel;

    for (i = 0; i <= node->height; i++)
        prev_nodes[i] = sentinel;

    /*Search level*/
    for (i = lv; i >= 0; i--) {
        while (head->next[i] && stoi(head->next[i]->record->get_id()) < id) {   
            head = head->next[i];
            //krata ta prougoumena nodes / path 
            //opws to video (ginontai antikatastaseis kathe fora)
            for (int j = head->height; j >= 0; j--) {   
                prev_nodes[j] = head;
            }
        }
    }

    //conenct all other nodes behind & forward the node!
    for (int j = node->height; j >= 0; j--) {
        node->next[j] = prev_nodes[j]->next[j];
        prev_nodes[j]->next[j] = node;          
    }

}



/*Delete the node with the referent to rec*/
void SkipList::Delete (Record* rec)
{
    int id = stoi(rec->get_id());
    Node* head = sentinel;
    Node* prev_nodes[sentinel->height+1];
    int i;
    for (int j = 0; j <= sentinel->height; j++)
        prev_nodes[j] = sentinel;

    for (i = sentinel->height; i >= 0; i--) 
    {
        while (head->next[i] && stoi(head->next[i]->record->get_id()) < id) {   
            head = head->next[i];
            for (int j = head->height; j >= 0; j--) {   
                prev_nodes[j] = head;
            }
        }

        if ( head->next[i] && (stoi(head->next[i]->record->get_id()) == id)) {
            head = head->next[i];
            break;
        }
    }

    for (int j = head->height; j >= 0; j--) {
        prev_nodes[j]->next[j] = head->next[j];
    }
    free (head->next);
    free (head);
    numNodes--;

}

//Epistrefei NULL an den brei tin eggrafh
Record* SkipList::get_record(string id_str)
{
    int i;
    Node* head = sentinel;
    int id = stoi(id_str);
    
    for (i = sentinel->height; i >= 0; i--) 
    {
        while (head->next[i] && (stoi(head->next[i]->record->get_id()) < id)) {   
            head = head->next[i];
        }

        if ( head->next[i] && (stoi(head->next[i]->record->get_id()) == id)) {
            return head->next[i]->record;
        }
    }

    return NULL;
}


/* Eistrefei ton arithmo twn politwn tis xwras country pou emvoliastikan sto [date1,date2] */
int SkipList::countStatusByCountry(char* country, Date* date1, Date* date2)
{
    int count = 0;
    Date* date;
    Node* head = sentinel->next[0];

    while (head != NULL)
    {
        if (!strcmp(country, head->record->get_country()))
        {
            date = head->record->check_vaccinated(virusName);
            if (date){
                if ( (*date1 <= *date) && (*date <= *date2) )
                {
                    count++;
                }
            }
        }
        head = head->next[0];
    }

    return count;
}


//epistrefei ton arithmo twn emvoliasmenwn gia tin xwra (ston io pou einai h skipList)
int SkipList::countStatusByCountry (char* country)
{
    int count = 0;
    Date* date;
    Node* head = sentinel->next[0];

    while (head != NULL)
    {
        if (!strcmp(country, head->record->get_country()))
        {
            date = head->record->check_vaccinated(virusName);
            if (date){      //an exei emvoliastei
                count++;
            }
        }
        head = head->next[0];
    }

    return count;
}



//Arithmos mh emvoliasmenwn gia thn xwra
int SkipList::countStatusByCountry_NoVacc(char* country)
{
    int count = 0;
    Node* head = sentinel->next[0];

    while (head != NULL)
    {
        if (!strcmp(country, head->record->get_country()))
        {
            count++;
        }
        head = head->next[0];
    }

    return count;
}


// Gia tous emvoliasmenous sto diasthma [date1,date2] me country
int* SkipList::countStatusBy_Age_Country (char* country, char* virusName, Date* date1, Date* date2)
{
    int* Ages = new int[4];        //0-20 && 20-40 && 40-60 && 60+
    Date* date = NULL;
    Node* head = sentinel->next[0];

    for (int i = 0; i < 4; i++)
        Ages[i] = 0;
    

    while (head != NULL)
    {
        if (!strcmp(country, head->record->get_country()))
        {
            date = head->record->check_vaccinated(virusName);

            if (date) {
                if ( ((*date1 <= *date) && (*date <= *date2)) && (head->record->get_age() < 20) )
                    Ages[0]++;

                if ( ((*date1 <= *date) && (*date <= *date2)) &&
                    (head->record->get_age() >= 20) && (head->record->get_age() < 40) )
                    Ages[1]++;

                if ( ((*date1 <= *date) && (*date <= *date2)) &&
                    (head->record->get_age() >= 40) && (head->record->get_age() < 60) )
                    Ages[2]++;

                if ( ((*date1 <= *date) && (*date <= *date2)) && (head->record->get_age() >= 60) )
                    Ages[3]++;
            }
        }
        head = head->next[0];
    }

    return Ages;
}


//gia tous total_num_yes sthn country  (den uparxei o periorismos tou date) 
int* SkipList::countStatusBy_Age_Country(char* country)      
{
    int* Ages = new int[4];        //0-20 && 20-40 && 40-60 && 60+
    Date* date = NULL;
    Node* head = sentinel->next[0];

    for (int i = 0; i < 4; i++)
        Ages[i] = 0;
    

    while (head != NULL)
    {
        if (!strcmp(country, head->record->get_country()))
        {
            date = head->record->check_vaccinated(virusName);

            if (date) {
                if ( head->record->get_age() < 20 )
                    Ages[0]++;

                if ( (head->record->get_age() >= 20) && (head->record->get_age() < 40) )
                    Ages[1]++;

                if ( (head->record->get_age() >= 40) && (head->record->get_age() < 60) )
                    Ages[2]++;

                if ( (head->record->get_age() >= 60) )
                    Ages[3]++;
            }
        }
        head = head->next[0];
    }

    return Ages;
}


// Gia tous mh emvoliasmenous  (den uparxei o periorismos tou date)
int* SkipList::countStatusBy_Age_Country_NoVacc (char* country)
{
    int* Ages = new int[4];        //0-20 && 20-40 && 40-60 && 60+

    for (int i = 0; i < 4; i++)
        Ages[i] = 0;

    Node* head = sentinel->next[0];

    while (head != NULL)
    {
        if (!strcmp(country, head->record->get_country()))
        {
            if ( (head->record->get_age() < 20) )
                Ages[0]++;

            if ( (head->record->get_age() >= 20) && (head->record->get_age() < 40) )
                Ages[1]++;

            if ( (head->record->get_age() >= 40) && (head->record->get_age() < 60) )
                Ages[2]++;

            if ( (head->record->get_age() >= 60) )
                Ages[3]++;
        }
        head = head->next[0];
    }

    return Ages;
}



void SkipList::print()
{
    cout << "\n ~~~~~~ Skip List for " << virusName <<" ~~~~~~~~~~ \n";
    Node* head;

    for (int i = 0; i <= sentinel->height; i++)
    {
        head = sentinel->next[i];
        cout << "Level " << i << ": ";
        while (head != NULL)
        {
            cout << head->record->get_id() << " ";
            head = head->next[i];
        }
        cout << "\n";
    }
}


int SkipList::print_citizens()
{
    Node* head = sentinel->next[0];
    int count = 0;
    while (head != NULL)
    {
        head->record->print_citizen();
        cout << endl;
        count++;
        head = head->next[0];
    }
    return count;
}