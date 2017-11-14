#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <sstream>
#include <set>
#include <string>
#include <list>
#include <iomanip>
#include "timer.h"
#include <map>
#include <algorithm>  
//#include<vector>

using namespace std;

//function prototypes
//set<set<int>> Apriori(ofstream & outfile, multiset<set<int>> DB, int maxTransSizeDB, int, set<set<int>>&); 
set<set<int>> Apriori(ofstream & outfile, multiset<set<int>> DB, int maxTransSizeDB, int, 
	map<set<int>, int> & LDBMap); 
multiset<set<int>> fileProcess(string filename, int &numDB, int &maxTransSizeDB, 
	map<int,set<int>>&); 
map<set<int>,int> readMapFile(string); 
map<int,set<int>> readTidlistMap(string);

set<set<int>> create1ItemCand(multiset<set<int>>); //create 1-itemset candidates set<set<int>>
set<set<int>> create1ItemCand(set<set<int>>); //create 1-itemset candidates set<set<int>>
set<int> create1ItemCandSet(multiset<set<int>>); //create 1-itemset candidates set<int>
set<int> create1ItemCandSet(set<set<int>>); //create 1-itemset candidates set<int>

set<set<int> > genCandidates(set<set<int> >); //generates all of the candidates

bool setInTrans(set<int>, set<int>); //checks whether a set is in a transaction or not.
int cnt(multiset<set<int> >, set<int>); //returns the count(occurrence) of a set in the database
int cnt(set<set<int> >, set<int>); //
bool areJoinable(set<int>, set<int>); //checks whether two sets are joinable
set<int> join(set<int>, set<int>); //for two sets to be joinable they have to have the same prefix except the last item
bool passSubsetCheck(set<set<int> >, set<int>); //a set to be candidate it has to pass subset check test

//Display prototypes
void displaySet(set<int>);
void displaySet(set<int>, double);
void displayMultiset(multiset<int>);
void displaySets(set<set<int> >);
void displayMultiset(multiset<set<int>>);
void displayMap(map<int,set<int>>);
void displayMap(map<set<int>, int> tidlistMap);
void writeMap(map<set<int>, int>, ofstream &); //LDB to file
void writeTidlistMap(map<int,set<int>> tidlistMap, ofstream & ofTidlist); //tidlist to file

set<set<int>> removeSetAndSupersets(set<set<int>>, set<int>);
set<set<int>> removeSupersets(set<set<int>>, set<int>);

bool isLargeInLDB(set<set<int>>, set<int>);
bool isLargeInLDBFind(set<set<int> > LDB, set<int> cset);
//bool isLargeCheckTidlist(map<int, set<int>> tidlist, set<int> X, int threshold);

int thresCalc(double, int);
//multiset<int> conv2(set<set<int>>);

int main()
{
	int thresINTDB =0;
    	double thresPercent = 50;
	int numDB =0;
	int maxTransSizeDB =0;

	string pathT = "max100";
	string pathI = "i100";
	string pathN = "10K";

	std::ostringstream strs;
	strs << thresPercent;
	std::string str = strs.str() + "%orj";
	
	ofstream outfile; //output file for Apriori results
	string path = "C:\\Users\\bpc\\Desktop\\data\\";

	outfile.open(path + "tests\\output\\Apriori\\"
		+ pathT + '_' + pathI + '_' + pathN + '_' + str + ".txt" ,ios::app);
//	
	ofstream ofLDBMap;//LDB to be written to a file
	ofLDBMap.open(path + "tests\\output\\LDBMap\\" 
		+ pathT + '_' + pathI + '_' + pathN + '_' + str + ".txt", ios::app);
//
	ofstream ofTidlist; //tidlist to be written to a file
	ofTidlist.open(path + "tests\\output\\Tidlist\\"
		+ pathT + '_' + pathI + '_' + pathN + '_' + str + ".txt", ios::app);
//
	//ofstream ofStats; //filesize to be written to a file
	//ofStats.open(path + "tests\\output\\stats\\"
	//	+ pathT + '_' + pathI + '_' + pathN + '_' + str + ".txt", ios::app);
//
	Timer tPart; //timer object for Apriori
	map<int, set<int>> tidlistDB; //tidlist declaration for DB
	map<set<int>, int> LDBMap; //Map to keep LDB and their counts
	set<set<int>> LDB; //Set to keep large itemsets
	
	//string filename = path + pathT + '\\' + pathN + '\\' + "full.txt";
	string filename = path + '\\' + "tests\\input\\DB.txt";
	//string filename = path + '\\' + "newdata\\T100n100nt10K.transactions.out";
	//string filename = "C:\\Users\\bpc\\Desktop\\data\\newdata\\t10ni1000NT10K.transactions.out";

	//string filename = path + "data\\full.txt";
	multiset<set<int>> DB = fileProcess(filename, numDB, maxTransSizeDB, tidlistDB);
	writeTidlistMap(tidlistDB, ofTidlist);//write tidlistDB map to file
	//ofStats<<numDB;
	//ofStats.close();

	thresINTDB = thresCalc(thresPercent, numDB);
	outfile<<'\n'<<filename<<endl;
	outfile<<"Threshold % :"<<thresPercent<<endl;
	outfile<<"Threshold # :"<<thresINTDB<<endl;	
	LDB.clear();
	tPart.Start();
	LDB = Apriori(outfile, DB, maxTransSizeDB, thresINTDB, LDBMap);
	tPart.Stop();
	
	std::cout << std::fixed << std::setprecision(2)
	                 << tPart.Elapsed() << " ms" << std::endl;	
	outfile<<std::fixed << std::setprecision(2)
	                 << tPart.Elapsed() << " ms" << std::endl;
	cout<<"\nThis is what we have in LDBMap and is written in file here.\n";
	displayMap(LDBMap);
	writeMap(LDBMap, ofLDBMap);
	cout<<"\nEnd of Apriori processing......\n"<<endl; 
	outfile<<"End of Apriori processing"<<endl;
	outfile<<"=========================";

	ofTidlist.close();
	ofLDBMap.close();
	outfile.close();
	system("PAUSE");
	return 0;
}

/////////////////////////
//function definitions //
/////////////////////////


//bool isLargeCheckTidlist(map<int, set<int>> tidlist, set<int> X, int thresINT)
//{
//	bool large = true;
//	int ctr = 0, sizeOfS =0;
//	int sizeOfX = X.size(); //size of the set that we'll compare.
//	if (sizeOfX == 1) //this is 1-itemset, so we just need to look at the size of the set in tidlist.
//	{
//		set<int>::iterator it = X.begin();
//		map<int, set<int>>::iterator itmap = tidlist.find(*it);
//		if (itmap != tidlist.end() && itmap->second.size()>=thresINT)
//			return true;
//		else 
//			return false;
//	}
//	else //that means X is n-itemset. We need to look at every item in 
//		//this itemset and look their intersection of their elements in tidlist. 
//		//Smallest intersection among them is the one we are looking for.
//	{		
//		int trans=0;
//		set<int> s, tmp;
//		
//		set<int>::iterator it = X.begin();
//		//cout<<"\nX is :";
//		//displaySet(X);
//		int i = *it;
//		map<int, set<int>>::iterator itmap1 = tidlist.find(i);
//		map<int, set<int>>::iterator itmap2 = tidlist.find(*(++it));
//			
//		set_intersection(itmap1->second.begin(), itmap1->second.end(), itmap2->second.begin(), 
//		itmap2->second.end(), std::inserter( s, s.begin() ));	
//		//cout<<"\ns :";
//		//displaySet(s);
//		ctr = s.size();
//		for (it++; it != X.end(); it++)
//		{	
//			itmap2 = tidlist.find(*it);
//			set_intersection(s.begin(), s.end(), itmap2->second.begin(), 
//				itmap2->second.end(), std::inserter( tmp, tmp.begin() ));	
//			//displaySet(tmp);
//			if (tmp.size() < ctr)
//				ctr = tmp.size();
//
//			trans++;
//			s = tmp; //replace the intersection set with the new one.
//			tmp.clear();
//		}
//	}	
//	if(ctr>=thresINT)
//		return true;
//	else
//		return false;	
//}

bool setInTrans(set<int> transaction, set<int> cset)
{
	for (set<int>::iterator it = cset.begin();  it != cset.end(); it++)
		if (transaction.count(*it) == 0)
			return false;	
	return true;
}

set<set<int> > genCandidates(set<set<int> > LDB) //genCandidates function. 
{
	set<int> candidateSet;
	set<set<int> > candidateSets;
//	
	for (set<set<int>>::iterator it1 = LDB.begin();it1!=LDB.end();it1++) //after all items in the set it still comes here
	{
		set<int> tmpSet1 = *it1;
		set<set<int>>::iterator tmp;
		tmp = it1; 
		tmp++; //tmp iterator used to make sure that it2 starts from the next set after it1 initially when the inner loop begins
		for (set<set<int>>::iterator it2 = tmp;it2!=LDB.end(); it2++) 
		{
			set<int> tmpSet2 = *it2;

			if (areJoinable(tmpSet1, tmpSet2))
			{
				candidateSet = join(tmpSet1, tmpSet2); //join them and store them in candidate set temporarily
				//displaySet(candidateSet);
				bool passedAllSubsetTest = true;
				set<int> subset = candidateSet;
				set<int> tmpset;
				for(set<int>::iterator it = subset.begin();it!=subset.end();it++) //subset check 
				{
					tmpset = subset;
					tmpset.erase(*it); //in each iteration, send n-1 of candidate set for subset check
					if (passSubsetCheck(LDB, tmpset))
						continue;
					else
					{
					//	cout<<"itemset did not pass subset tests"<<endl;
						passedAllSubsetTest = false;	
						candidateSet.clear();
						break;
					}				
				}
				if (passedAllSubsetTest)
				{
			//	cout<<"passed all subset tests"<<endl;
				candidateSets.insert(candidateSet); //if joinable and pass all subset tests, then, insert the joined set into candidate sets.
				//displaySet(candidateSet);
				}
				candidateSet.clear();
				//tmpSet1.clear();
				tmpSet2.clear();
			//	cout<<"passedAllSubsetTest flag:"<<passedAllSubsetTest<<endl;
			}
			else
			{
			//	cout<<"they are not joinable"<<endl;
				tmpSet1.clear();
				tmpSet2.clear();
				candidateSet.clear();
				//it1++; //increment the iterator in LDB
				break;
			}
		}
	}
	//displaySets(candidateSets);
	return candidateSets; //candidateSets;
}

////checks whether two sets are joinable --i.e. if their all prefixes are the same and only the last item is different they are joinable.
bool areJoinable(set<int> set1, set<int> set2) 
{
	set<int>::iterator it2 = set2.begin();
	for(set<int>::iterator it1 = set1.begin(); it1!= --set1.end(); it1++, it2++)
	{
		if (*it1 != *it2)
			return false;
	}
	return true;	
}
//	
bool passSubsetCheck(set<set<int> > LDB, set<int> set1) 
{
	if (LDB.count(set1)) //if this returns anything other than zero, it is true
		return true;
	else
		return false;
}

//join two sets and return  the joined set --i.e. join function adds the last item of set2 to set1 and returns set1.
set<int> join(set<int> set1, set<int> set2)
{	
	std::set<int>::reverse_iterator rit;
	rit = set2.rbegin();
   	set1.insert(*rit); // joined set1 and set2
	return set1;
}
//
////display the content of a single set
void displaySet(set<int> set1, double cnt) 
{
	cout<<cout<<"\ndisplaySet and cnt..\n";
	for (set<int>::iterator it=set1.begin(); it!=set1.end(); it++)
 	{
		cout<<*it<<' ';
	}
	cout<<"\t"<<cnt<<endl;
}

void displaySet(set<int> set1) 
{
	cout<<cout<<"\ndisplaySet...\n";
	for (set<int>::iterator it=set1.begin(); it!=set1.end(); it++)
 	{
		cout<<*it<<' ';
	}
	//cout<<"\t"<<cnt<<endl;
}

void displayMultiset(multiset<int> set1) 
{
	cout<<"\ndisplayMultiset<int>...\n";
	cout<<'\n';
	for (multiset<int>::iterator it=set1.begin(); it!=set1.end(); it++)
 		{
			cout << " :" << *it;
		}
		cout << " " << endl;
}

//display the contents of a set of sets
void displaySets(set<set<int> > sets) 
{
	cout<<"\ndisplaySets...\n";
	if (sets.size()>0)
	{
		for (set<set<int> >::iterator it2=sets.begin(); it2!=sets.end(); it2++)
		{
 			set<int> cset = *it2;		
			for (set<int>::iterator it=cset.begin(); it!=cset.end(); it++)
 			{
				//cout << " :" << *it;
				cout <<*it<<' ';
			}
			cout << " " << endl;
  		}
	}
	else
		cout<<"{}"<<endl;
}
//
///given a transaction list and a set it counts and returns its occurrence.
int cnt(multiset<set<int> > DB, set<int> cset) 
{
	int count =0;
	
	set<int>::iterator it_cset = cset.begin();
	//find frequent itemsets
	 
	for (multiset<set<int>>::iterator it2=DB.begin(); it2!=DB.end(); it2++)
	{
		set<int> transaction = *it2;	
		set<int>::iterator it_transaction = transaction.begin();
		if (*it_transaction > *it_cset)	
			break; //since multiset is sorted, no other transactions may contain the set
		if (setInTrans(transaction, cset))
		{
			count++;
		}						
  	}
	return count;
}

bool isLargeInLDBFind(set<set<int> > LDB, set<int> cset) 
{
	set<set<int>>::iterator itk;
	itk = LDB.find(cset);
	
	if (itk == LDB.end())
		return false;
	else
		return true;
}

//display multiset
void displayMultiset(multiset<set<int>> DB)
{
	cout<<"\ndisplayMultiset<set<int>>...\n";
	for (multiset<set<int> >::iterator it2=DB.begin(); it2!=DB.end(); it2++)
	{
 		set<int> transaction = *it2;	

		for (set<int>::iterator it=transaction.begin(); it!=transaction.end(); it++)
 		{
			cout << " " << *it;
		}
		cout << " " << endl;
  	}
}

//display map
void displayMap(map<int, set<int>> tidlistMap)
{
	// showing contents:
	map<int,set<int>>::iterator it;
	string tmp;
	std::cout << "\ntidlistdb contains:\n";
	for (it=tidlistMap.begin(); it!=tidlistMap.end(); ++it)
	{
		 std::cout << it->first << " => ";
		 for (set<int>::iterator its=it->second.begin();its!=(it->second.end());its++)
		 {
			cout<<*its<<' ';
		 }
		 cout<<" size :"<<it->second.size()<<endl;
	}
}

void displayMap(map<set<int>, int> LDBMap)
{
	// showing contents:
	map<set<int>, int>::iterator it;
	string tmp;
	std::cout << "\nLDBMap contains:\n";
	for (it=LDBMap.begin(); it!=LDBMap.end(); ++it)
	{		 
		 for (set<int>::iterator its=it->first.begin();its!=(it->first.end());its++)
		 {
			cout<<*its<<' ';
		 }
		  std::cout <<" => size :"<< it->second <<endl;
		 //cout<<" size :"<<it->second.size()<<endl;
	}
}

void writeMap(map<set<int>, int> LDBMap, ofstream & ofLDBMap)
{
	// showing contents:
	map<set<int>, int>::iterator it;
	//string tmp;
	//std::cout << "\nLDBMap contains:\n";
	for (it=LDBMap.begin(); it!=LDBMap.end(); ++it)
	{		 
		 for (set<int>::iterator its=it->first.begin();its!=(it->first.end());its++)
		 {
			ofLDBMap<<*its<<' ';
		 }
		  ofLDBMap<< it->second <<endl;		 
	}
	ofLDBMap.close();
}

void writeTidlistMap(map<int,set<int>> tidlistMap, ofstream & ofTidlist)
{
	// showing contents:
	map<int,set<int>>::iterator it;
	
	for (it=tidlistMap.begin(); it!=tidlistMap.end(); ++it)
	{		 
		ofTidlist<< it->first<<' ';
		 for (set<int>::iterator its=it->second.begin();its!=(it->second.end());its++)
		 {
			ofTidlist<<*its<<' ';
		 }	
		 ofTidlist<<'\n';
	}
	ofTidlist.close();
}

//Process file
multiset<set<int>> fileProcess(string filename, int &numdb, int &maxTransSizeDb, 
	map<int,set<int>> & tidlistMap)
{
	ifstream inFile;
	inFile.open(filename.c_str());
	if (inFile.fail())  // check for successful open
	{
		cout<< "\nThe file was not successfully opened"
		<< "\n Please check that the file currently exists."
		<< endl;
		exit(1);
	}
	cout<<"\nprogram is in fileprocess.."<<endl;
	multiset<set<int> > tmpDb;
	string line;
	int transCtr =0;
	while(getline(inFile, line) && line.length() >0)
	{
		transCtr++;
		set<int> transaction;
		set<int> mapSet;
		std::map<int,set<int>>::iterator it;
		mapSet.insert(transCtr);
		int result;
		string tmpStr ="";
			
		for(int i=0;i<line.length();i++)
		{
			if (!isspace(line[i]))
			{
				tmpStr +=line[i]; 
			}
			else
			{	
				stringstream(tmpStr)>> result;
				transaction.insert(result);
				std::map<int,set<int>>::iterator it1;
				it1 = tidlistMap.find(result);
				if (it1 == tidlistMap.end() )				
					tidlistMap.insert (pair<int, set<int>>(result, mapSet));
				else
					if (it1 ->first == result)
						it1 ->second.insert(transCtr);				
					else
						cout<<"\nstill there is a problem!";
								
				//tidlistdb.insert(transCtr)->second;				
				tmpStr ="";
		 	}
		}
		//cout<<endl;
		stringstream(tmpStr)>> result; //last item after the last space
		transaction.insert(result);
		tmpDb.insert(transaction); //transaction is inserted into DB
		//insert the last element of the transaction into the map
		std::map<int,set<int>>::iterator it2;
		it2 = tidlistMap.find(result);
		if (it2 == tidlistMap.end() )				
					tidlistMap.insert (pair<int, set<int>>(result, mapSet));
				else
					if (it2 ->first == result)
						it2 ->second.insert(transCtr);				
					else
						cout<<"\nstill there is a problem!";
		
		numdb++;
		   
		if (transaction.size() > maxTransSizeDb)
		{
			maxTransSizeDb = transaction.size();			
		}
	}
	 // showing contents:
	//displayMap(tidlistMap);
	
	inFile.close(); 

	cout<<"\nNumber of transactions in dataset "<<filename<<" is "<<numdb<<endl;
	cout << "\nAll transactions in dataset "<<filename<<" contains: "<<endl;
	//displayMultiset(tmpDb);
	return tmpDb;
}

//read a map file into tidlist
map<set<int>,int> readMapFile(string filename)
{
	ifstream inFile;
	inFile.open(filename.c_str());
	if (inFile.fail())  // check for successful open
	{
		cout<< "\nThe file was not successfully opened"
		<< "\n Please check that the file currently exists."
		<< endl;
		exit(1);
	}
	map<set<int>,int> tmpMap;
	string line;
	int transCtr =0;
	while(getline(inFile, line))
	{
		transCtr++;
		set<int> transaction;
		//std::map<int,set<int>>::iterator it;
		
		int result;
		string tmpStr ="";
		int i;
		for(i=0;i<line.length()-1;i++)
		{
			if (!isspace(line[i]))
			{
				tmpStr +=line[i]; 				
			}
			else
			{	
				stringstream(tmpStr)>> result;
				transaction.insert(result);								
				tmpStr ="";
		 	}		
		}
		tmpStr =line[i]; 
		stringstream(tmpStr)>> result;		
		//insert the last element of the transaction into the map
		tmpMap.insert (pair<set<int>,int>(transaction,result));
		tmpStr ="";
		transaction.clear();
	}
	inFile.close(); 
	cout<<"..\nReading stored large items file into LDBMap"<<endl;
	displayMap(tmpMap);
	return tmpMap;
}

//read a map file into tidlist
map<int,set<int>> readTidlistMap(string filename)
{
	ifstream inFile;
	inFile.open(filename.c_str());
	if (inFile.fail())  // check for successful open
	{
		cout<< "\nThe file was not successfully opened"
		<< "\n Please check that the file currently exists."
		<< endl;
		exit(1);
	}
	map<int, set<int>> tmpMap;
	string line;
	int transCtr =0;
	while(getline(inFile, line))
	{
		transCtr++;
		set<int> transaction;
		//std::map<int,set<int>>::iterator it;
		
		int result,tmp;
		string tmpStr ="";
		int i=0;
		tmpStr =line[i]; 
		stringstream(tmpStr)>> result;
		tmpStr ="";
		for(i=1;i<line.length();i++)
		{
			if (!isspace(line[i]))
			{
				tmpStr =line[i]; 		
				stringstream(tmpStr)>> tmp;
				transaction.insert(tmp);
			}				
		}				
		//insert the last element of the transaction into the map
		tmpMap.insert (pair<int, set<int>>(result, transaction));
		tmpStr ="";
		transaction.clear();
	}
	inFile.close(); 
	cout<<"..\nBegin reading stored tidlist file..."<<endl;
	displayMap(tmpMap);
	cout<<"..\nEnd reading stored tidlist file..."<<endl;
	return tmpMap;
}

//
//Apriori
//set<set<int>> Apriori(multiset<set<int>> DB, int maxTransSizeDB, vector<set<int>> &v, int thresINT, set<set<int>> &NDB) 
set<set<int>> Apriori(ofstream & outfile, multiset<set<int>> DB, int maxTransSizeDB, int thresINT, map<set<int>, int> & LDBMap) 
{
	set<set<int>> candiSet = create1ItemCand(DB);
	set<set<int>> LDB;
	set<int> mapSet;

	outfile<<"Apriori LDB..."<<endl;
	for (int i = 1; i<= maxTransSizeDB; i++)
	{
		if ( candiSet.size() < 1 )
		{ 
			//cout<<"{}"<<endl; 
			cout<<endl;
			break;
		}
		
		set<set<int>>::iterator it = candiSet.begin();

		set<set<int>> frequentSet;		
		
		///
		for (set<set<int>>::iterator it = candiSet.begin(); it!=candiSet.end();it++)
		{	
			double count = cnt(DB, *it);
			//double count = DB.count(*it);
			if (count >= thresINT)
			{
				frequentSet.insert(*it);
				LDB.insert(*it);
				mapSet = *it;
				LDBMap.insert (pair<set<int>, int>(mapSet,count));	
			}
		}
		candiSet = genCandidates(frequentSet); //given a frequent set generate new set of candidate itemsets
	}

	 // showing LDBMap contents:
	cout<<"\nLDBMap...\n";
	displayMap(LDBMap);
	
	return LDB;
	//return ls;

}
//create 1-item candidates
set<set<int>> create1ItemCand(multiset<set<int>> DB)
{
	set<set<int>> candiSet; //set of set for all candidates
	for (multiset<set<int> >::iterator it = DB.begin(); it!=DB.end();it++) //browse through database
		for (set<int>::iterator tItem = (*it).begin(); tItem!=(*it).end(); tItem++) //process each transaction
		{
			set<int> c; c.insert(*tItem);
			candiSet.insert(c);
		}
	return candiSet;
}

//create 1-item candidates set<int>
set<int> create1ItemCandSet(multiset<set<int>> DB)
{
	set<int> candiSet; //set of set for all candidates
	for (multiset<set<int> >::iterator it = DB.begin(); it!=DB.end();it++) //browse through database
		for (set<int>::iterator tItem = (*it).begin(); tItem!=(*it).end(); tItem++) //process each transaction
		{
			candiSet.insert(*tItem);
		}
	return candiSet;
}

//create 1-item candidates set<int>
set<int> create1ItemCandSet(set<set<int>> DB)
{
	set<int> candiSet; //set of set for all candidates
	for (set<set<int> >::iterator it = DB.begin(); it!=DB.end();it++) //browse through database
		for (set<int>::iterator tItem = (*it).begin(); tItem!=(*it).end(); tItem++) //process each transaction
		{
			candiSet.insert(*tItem);
		}
	return candiSet;
}

//create 1-item candidates
set<set<int>> create1ItemCand(set<set<int>> DB)
{
	set<set<int>> candiSet; //set of set for all candidates
	for (set<set<int> >::iterator it = DB.begin(); it!=DB.end();it++) //browse through database
		for (set<int>::iterator tItem = (*it).begin(); tItem!=(*it).end(); tItem++) //process each transaction
		{
			set<int> c; c.insert(*tItem);
			candiSet.insert(c);
		}
	return candiSet;
}
//pruneSet
set<set<int>> subtractSet(set<set<int>> L1DB, set<set<int>> C1db)
{
	//cout<<"\nL1DB : \n"; displaySets(L1DB);
	//cout<<"\nC1db : \n"; displaySets(C1db);
	
	set<set<int>> pruneSet;
	for(set<set<int>>::iterator it = L1DB.begin(); it !=L1DB.end(); it++)
	{
		if (C1db.count(*it) >0)		 // ***change count to find or something to stop counting after finding membership.
			continue;
		else
			pruneSet.insert(*it);
	}
	cout<<"\nPrune set : "<<endl;
	displaySets(pruneSet);
	return pruneSet;
}

//remove an itemset and all of its supersets from large itemsets
set<set<int>> removeSetAndSupersets(set<set<int>> sets, set<int> item)  ////how about item??????
{
 	set<set<int>> tmpSet = sets;
	for (set<set<int>>::iterator it=sets.begin(); it!=sets.end(); it++)
 	{
		set<int> s = *it;
		bool allItemsInSet = true;
		for (set<int>::iterator it2=item.begin(); it2!=item.end(); it2++)
		{
			if (s.count(*it2))
			{
				continue;				
			}
			else
			{
				allItemsInSet = false;
				break;
			}				
		}
		if (allItemsInSet)
		{
			tmpSet.erase(*it);
		}
	}
	cout << " " << endl;
	return tmpSet;	
}

//remove all of supersets of an itemset from large itemsets
set<set<int>> removeSupersets(set<set<int>> sets, set<int> item)  
{
 	set<set<int>> tmpSet = sets;
	for (set<set<int>>::iterator it=sets.begin(); it!=sets.end(); it++)
 	{
		set<int> s = *it;
		bool allItemsInSet = true;
		for (set<int>::iterator it2=item.begin(); it2!=item.end(); it2++)
		{
			if (s.count(*it2))
			{
				continue;				
			}
			else
			{
				allItemsInSet = false;
				break;
			}				
		}
		if (allItemsInSet &&  *it !=item)
		{
			tmpSet.erase(*it);
		}
	}
	cout << " " << endl;
	return tmpSet;	
}

//add all superset of a set to a set
//void addSupersets(set<set<int>> LDB, set<set<int>> &pSet, set<int> X) 
//{ 			
//	for (set<set<int>>::iterator it=LDB.begin(); it!=LDB.end(); it++)
// 	{
//		set<int> s = *it;
//		bool supersetExist = true;
//		for(set<int>::iterator it2=X.begin();it2!=X.end();it2++)
//		{
//			if (s.count(*it2))
//			{
//				continue;
//			}
//			else
//			{
//				supersetExist = false;
//				break;
//			}
//		}
//		if (supersetExist)
//				pSet.insert(*it);
//	}
//	cout << " " << endl;
//}

//combineSets
//multiset<set<int>> combineSets(multiset<set<int>> DB, multiset<set<int>> db)
//{
//	multiset<set<int>> mSet;
//	mSet = DB;
//	for(multiset<set<int>>::iterator it = db.begin(); it !=db.end(); it++)
//	{
//		mSet.insert(*it);
//	}
//	return mSet;
//}
//
//bool isSmall(multiset<set<int>> mSet, set<int> s, int thresINT)
//{
//	//displaySet(s);
//	//cout<<"\nset : "<<s<<" count :"<<cnt(DBdb, s)<<endl;
//	return cnt(mSet, s) < thresINT;
//	//return mSet.count(s)< thresINT;
//}
//
//bool isLarge(multiset<set<int>> mSet, set<int> s, int thresINT)
//{
//	//displaySet(s);
//	//cout<<"\nset : "<<s<<" count :"<<cnt(DBdb, s)<<endl;
//	return cnt(mSet, s) >= thresINT;	
//	//return mSet.count(s) >= thresINT;
//}

int thresCalc(double thresPercent, int num)
{
	int percent = thresPercent * num;
	if (percent % 100 == 0)
		return (thresPercent * num)/100;
	else
		return ((thresPercent * num)/100) +1;
}
//
//converts set of sets to a multiset
multiset<int> conv2(set<set<int>> DB)
{	
	multiset<int> tmpDB;
	for (set<set<int> >::iterator it2=DB.begin(); it2!=DB.end(); it2++)
	{
 		set<int> transaction = *it2;		
		for (set<int>::iterator it=transaction.begin(); it!=transaction.end(); it++)
 		{
			tmpDB.insert(*it);//insert all transactions into a multiset
		}
	}
	return tmpDB;
}
