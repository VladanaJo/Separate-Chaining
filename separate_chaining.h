#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>



 template <typename Key, size_t N = 7>   


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

    size_type tableRowSize = N;         
    size_type currentSize = 0;          

    struct Cell{                       
        Cell *start = nullptr;        
        Cell *next = nullptr;          
        value_type elem;              

        ~Cell(){           
           delete start;   
           delete next;     
        }
    };

    Cell *table = nullptr;              

    
   

//-------------------------  R E H A S H  -------------------------------                     

   void rehash(){

      key_type *newElem = new key_type[currentSize];
      size_type rowNumber=0;
      size_type i = 0;
      Cell *j = table[i].start;
      size_type k = 0;

      while(i<tableRowSize){        
        j = table[i].start;                                          
        while(j != nullptr){              
          newElem[rowNumber]= j->elem;  
          ++rowNumber;                  
          j = j->next;    
         }
         ++i;        
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
     }                                            



//------------------  I N S E R T   U N C H E C K E D  ------------------
   void insert_unchecked(const_reference elem){         
    size_type num = hashFunktion(elem);
    Cell *newCell = new Cell;                           

    newCell->next = table[num].start;                   
    newCell->elem = elem;                               

    table[num].start = newCell;                         
    currentSize++;                                      

    if(tableRowSize*7<currentSize)
      rehash();                                     
   

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
      return !currentSize;        
  }                               

  //---------------  I N S E R T   ----------------------------
  void insert(std::initializer_list<key_type> ilist){ 
    for(auto& i:ilist)                  
        if(count(i)==0)                 
          insert_unchecked(i);         
  }

  //------------    INSERT   with   F I R S T - L A S T  --------------------------
  template<typename InputIt> void insert(InputIt first, InputIt last){
    auto i = first;
    while(i != last){           
      if(count(*i)==0)          
        insert_unchecked(*i);   
      ++i;
    }
  }

  //-----------   C O U N T   --------------------------------
  size_type count(const key_type &key) const{
    const auto c = hashFunktion(key);  
    Cell *i = table[c].start;
    while(i != nullptr){             
      if(key_equal{}(i->elem, key))  
        return 1;                    
      i = i->next;
    }                      
   
    return 0;                       
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
      insert(i);         
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
  iterator find(const key_type& key) const{      
    
    if(count(key)==0) 
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
    if(it == iterator{nullptr})   
      return 0;                 

    bool theFirstOne{true};
    size_type row = hashFunktion(key);
    auto* firstCheck = table[row].start;
    auto* secondCheck = table[row].start;

    while(firstCheck != nullptr){
        if(key_equal{}(key, firstCheck->elem)){

          if(firstCheck == table[row].start){
              currentSize--;
              
              if(firstCheck->next == nullptr){  
                delete table[row].start;
                table[row].start = nullptr;
                return 1;
              }

             
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

      if(theFirstOne == false)          
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
    
    if(start->next == nullptr){
        ++row;                       
        
        while(row < tableRowSize && table[row].start == nullptr){  
          ++row;
        }
        
        if(tableRowSize <= row){    
          start = nullptr;             
          return *this;
        }
        start = table[row].start;   
        return *this;
    }

    
    if(start->next != nullptr){
      start = start->next;           
      return *this;
    } 
  }

  Iterator operator++(int) {
    Iterator now(*this);
    operator++();
    return now;
}


  friend bool operator==(const Iterator& lhs, const Iterator& rhs){ 
      return lhs.start == rhs.start;        
  }
  

  friend bool operator!=(const Iterator& lhs, const Iterator& rhs){ 
      return !(lhs.start == rhs.start);    
  }
  

};

template <typename Key, size_t N> void swap(ADS_set<Key,N>& lhs, ADS_set<Key,N>& rhs){ //Methode swap()
     lhs.swap(rhs);  
}

#endif 