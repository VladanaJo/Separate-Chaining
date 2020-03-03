#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

//--------------------------------------------------    P H A S E   1    ----------------------------------------------------------------


 template <typename Key, size_t N = 7>   //N je broj mijesta i koristi se za rehash


class ADS_set {
public:
class Iterator;
using value_type = Key;
using key_type = Key;
using reference = key_type&;
using const_reference = const key_type&;       
using size_type = size_t;
using difference_type = std::ptrdiff_t;
using iterator = Iterator;
using const_iterator = Iterator;
using key_compare = std::less<key_type>;   
using key_equal = std::equal_to<key_type>; 
using hasher = std::hash<key_type>;        

 private:

    size_type tableRowSize = N;         // number of rows
    size_type currentSize = 0;          

    struct Cell{                       // input
        Cell *start = nullptr;         // first elem 
        Cell *next = nullptr;          // nextelement
        value_type elem;               // elem

        ~Cell(){           
           delete start;   
           delete next;     
        }
    };

    Cell *table = nullptr;              // pokazivač na objekt

    
   

//-------------------------  R E H A S H  -------------------------------    // there is no space in table                  

   void rehash(){

      key_type *newElem = new key_type[currentSize];
      size_type rowNumber=0;
      size_type i = 0;
      Cell *j = table[i].start;
      size_type k = 0;

      while(i<tableRowSize){        
        j = table[i].start;                                          
        while(j != nullptr){            // while it not comes to the end   
          newElem[rowNumber]= j->elem;  
          ++rowNumber;                  // index se poveca, jer ce myArray narednog puta dobiti novi clan na mjestu numa
          j = j->next;    
         }
         ++i;          // next row 
      }    
      delete [] table;

      currentSize = 0;   
      tableRowSize *= 2;                    
      table = new Cell[tableRowSize];                          

      while(k<rowNumber){
        insert_unchecked(newElem[k]);
        ++k;
      } 

    delete[] newElem;

  }



//-------------------  H A S H   F U N K T I O N   ----------------------- 
   size_type hashFunktion(const key_type &key) const{  
     return hasher{}(key) % tableRowSize;
     }                                            // koristi hasher{} jer može biti string



//------------------  I N S E R T   U N C H E C K E D  ------------------
   void insert_unchecked(const_reference elem){         //ubaciti vrijednosti u tabelu
    size_type num = hashFunktion(elem);
    Cell *newCell = new Cell;                           //napravi novu Cell

    newCell->next = table[num].start;                   //varijabilni položaj prima indeks na koji treba postaviti varijabilni elem
    newCell->elem = elem;                               //newCell dobija sadržaj

    table[num].start = newCell;                         //bas na ovoj poziciji (num) 
    currentSize++;                                      

    if(tableRowSize*7<currentSize)
      rehash();                                         // kada je tabela već prekoračena, rehash je pozvan da poveca tabelu
   

  }

 public:

 //------------------  K O N S T R U K T O R  -  D E F A U L T  ----------------------
  ADS_set(){ 
    table= new Cell[tableRowSize];        
  }

 // ----------------   K O N S T R U K T O R  -  I L I S T ---------------------------
 ADS_set(std::initializer_list<key_type> ilist): ADS_set(){    
    insert(ilist);                      
  }

 // -------------   K O N S T R U K T O R  -  F I R S T - L A S T  --------------------
  template<typename InputIt> ADS_set(InputIt first, InputIt last) : table(new Cell[N]){   
    insert(first,last);              
  }

  //------------- D E S T R U K T O R --------------------------
  ~ADS_set(){ 
    delete [] table;            
  }

  //---------------   S I Z E  ---------------------------------
  size_type size() const{               
     return currentSize;          
  }

  //---------------   E M P T Y  -------------------------------
  bool empty() const{             
      return !currentSize;        // if size==2 (not empty), it returns -2 (0) --> false
  }                               // if size==0   (empty),   it returns    1   --> true

  //---------------  I N S E R T   ----------------------------
  void insert(std::initializer_list<key_type> ilist){ 
    for(auto& i:ilist)                  
        if(count(i)==0)                 // thereis no every ilist in the list, than
          insert_unchecked(i);          // add it
  }

  //------------    INSERT   with   F I R S T - L A S T  --------------------------
  template<typename InputIt> void insert(InputIt first, InputIt last){
    auto i = first;
    while(i != last){           
      if(count(*i)==0)          // there is no in table
        insert_unchecked(*i);   // add it
      ++i;
    }
  }

  //-----------   C O U N T   --------------------------------
  size_type count(const key_type &key) const{
    const auto c = hashFunktion(key);  // becomes key variable
    Cell *i = table[c].start;
    while(i != nullptr){             // check to the end
      if(key_equal{}(i->elem, key))  // it finds element, thats like key
        return 1;                    // found!
      i = i->next;
    }                      
   
    return 0;                        // not found!
  }

 //-----------------   D U M P   ------------------------------
  void dump(std::ostream& os = std::cerr) const{
   os << "Separate Chaining Tabelle: \n";
   os << "Number of inputs = " << currentSize << ", number of rows = " << tableRowSize << "\n \n"; 
   
   size_type i = 0;
   while(i<tableRowSize){   
      os << "Row = " << i << "  -->   ";  
      auto* j=table[i].start;             
      while(j != nullptr){
          if(j->next!=nullptr)                          
            os << " ";                            
          if(j == nullptr)
            continue;     
          os << j->elem; 
          j = j->next;       
      }
    os << "\n";                 
    ++i;                        
    }
  }


//-------------------------  ABGABE 2 -------------------------------


// ----------------  A D S _ S E T (o t h e r) ----------------
  ADS_set(const ADS_set& other) : table(new Cell[N]){  
      for(const auto &i : other){
        insert_unchecked(i);
      }
  }

  // ---------------  O P E R A T O R = (o t h e r) ---------------
  ADS_set& operator=(const ADS_set& other){ 
    clear();  

    for(auto &i: other){ 
      insert(i);         //insert everything from other
    }  

    return *this;
  }

  // ---------------  O P E R A T O R = (i l i s t) ---------------
  ADS_set& operator=(std::initializer_list<key_type> ilist){
    clear();                       
    insert(ilist);              
    return *this;              
  }

// --------------------- F I N D (k e y)  -----------------------
  iterator find(const key_type& key) const{      //return iterator on found element
    
    if(count(key)==0) //wenn nicht gefunden
      return iterator{nullptr};

    const auto i = hashFunktion(key);
    Cell *j = table[i].start;
    while(j != nullptr){
      if(key_equal{}(j->elem, key))
        return iterator(table, j, i, tableRowSize);
      j = j->next;
    }
    return iterator{nullptr};
  }


  // -----------------------  C L E A R --------------------------
  void clear(){   
       delete [] table;
       tableRowSize = N;
       table = new Cell[tableRowSize];
       currentSize = 0;
  }

// --------------------  S W A P (o t h e r) -------------------
  void swap(ADS_set& other){
      using std::swap;
      swap(table, other.table);
      swap(currentSize, other.currentSize);
      swap(tableRowSize, other.tableRowSize);
  }

  // --------------------  I N S E R T (k e y) -------------------
  std::pair<iterator,bool> insert(const key_type& k){ 
      using std::make_pair;
      if(count(k)==1)
        return make_pair(find(k), false);

      if(count(k)==0){
        insert_unchecked(k);
        return make_pair(find(k), true);
      }
  }

  // ---------------------  E R A S E (k e y) -------------------- 
  size_type erase(const key_type& key){
    auto it = find(key);
    if(it == iterator{nullptr})    // if there are no such entries
      return 0;                    // there is nothing to delete

    bool theFirstOne{true};
    size_type row = hashFunktion(key);
    auto* firstCheck = table[row].start;
    auto* secondCheck = table[row].start;

    while(firstCheck != nullptr){
        if(key_equal{}(key, firstCheck->elem)){

          if(firstCheck == table[row].start){
              currentSize--;
              // there is no next number
              if(firstCheck->next == nullptr){  
                delete table[row].start;
                table[row].start = nullptr;
                return 1;
              }

              // there is a next number
              table[row].start = firstCheck->next;
              firstCheck->next = nullptr;
              delete firstCheck;
              return 1;
          }

          currentSize--;
          secondCheck->next = firstCheck->next;
          firstCheck->next = nullptr;
          secondCheck = nullptr;

          delete firstCheck;
          delete secondCheck;

          return 1;
        }

      if(theFirstOne == false)          // happens after the first time
        secondCheck = secondCheck->next;
      
      theFirstOne = false;
      firstCheck = firstCheck->next;
    }
    return 0;
}



  // -------------------------  B E G I N ------------------------
  const_iterator begin() const{ 
    size_type i = 0;
    while(i < tableRowSize){
      if(table[i].start)
        return  const_iterator(table, table[i].start, i, tableRowSize);
      ++i;
    }
    
    return const_iterator{nullptr};
  }



 // -----------------------  E N D  ---------------------------
  const_iterator end() const{
     return const_iterator{nullptr};
  } 

// -------------------  O P E R A T O R = = ---------------------
  friend bool operator==(const ADS_set& lhs, const ADS_set& rhs){
    if(lhs.currentSize == rhs.currentSize)
      for(auto &i : rhs)
        if(lhs.count(i) == 0)
          return false;
    
    if(lhs.currentSize != rhs.currentSize)
       return false;

    return true;
  }

  // ------------------  O P E R A T O R ! =  ---------------------
  friend bool operator!=(const ADS_set& lhs, const ADS_set& rhs){
         return !(lhs==rhs);
  }


};

// ------------------------------------- I T E R A T O R --------------------------------------------


template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator {

private:
   
   Cell *table = nullptr;
   Cell *start = nullptr;
   size_type row;
   size_type tableRowSize;

public:
  using value_type = Key;
  using difference_type = std::ptrdiff_t;
  using reference = const value_type&;
  using pointer = const value_type*;
  using iterator_category = std::forward_iterator_tag;

explicit Iterator (Cell *table = nullptr, Cell *start = nullptr, size_type row = 0, size_type tableRowSize = 0)
                  : table(table), start(start), row(row), tableRowSize(tableRowSize){}

  pointer operator->() const {
    return &start->elem;
  }

  reference operator*() const {
      return start->elem;
  }

  Iterator& operator++() {
    // the next input doesnt exist
    if(start->next == nullptr){
        ++row;                       // go to the next row
        
        while(row < tableRowSize && table[row].start == nullptr){  // if there is no inputs in rows, we go to the next row
          ++row;
        }
        
        if(tableRowSize <= row){     // it comes to the end, and we could just go to the nullptr
          start = nullptr;             
          return *this;
        }
        start = table[row].start;    // ++ successfully done
        return *this;
    }

    // the next input exit
    if(start->next != nullptr){
      start = start->next;           // and that is now start
      return *this;
    } 
  }

  Iterator operator++(int) {
    Iterator now(*this);
    operator++();
    return now;
}


  friend bool operator==(const Iterator& lhs, const Iterator& rhs){ 
      return lhs.start == rhs.start;        // ako su jednaki, returnace true
  }
  

  friend bool operator!=(const Iterator& lhs, const Iterator& rhs){ 
      return !(lhs.start == rhs.start);    // ako su razliciti returnace true
  }
  

};

template <typename Key, size_t N> void swap(ADS_set<Key,N>& lhs, ADS_set<Key,N>& rhs){ //Methode swap()
     lhs.swap(rhs);    //pozvati swap s lhs i parametrom rhs
}

#endif // ADS_SET_H