#include <iostream>

#include "tsf.h"

using namespace std;

ID_STR(Person);
ID_STR(Date);

ID_STR(firstName);
ID_STR(lastName);
ID_STR(birthday);
ID_STR(day);
ID_STR(month);
ID_STR(year);
ID_STR(friends);

void writeTsf()
{
    RootNode r;

    //////////////////

    auto person = r.add("",_Person);

    person->set(_firstName,"Anton");
    person->set(_lastName,"Anders");

    auto birthday = person->add(_birthday,_Date);
    birthday->set(_day,"25");
    birthday->set(_month,"11");
    birthday->set(_year,"1982");

    auto friends = person->add(_friends);
    friends->add("","","Berta Bittrich");
    friends->add("","","Carsten Claus");

    //////////////////

    person = r.add("",_Person);

    person->set(_firstName,"Daniel");
    person->set(_lastName,"Dietrich");

    birthday = person->add(_birthday,_Date);
    birthday->set(_day,"12");
    birthday->set(_month,"3");
    birthday->set(_year,"1995");

    //////////////////

    BoolStatus result = toTsf(r,"example.tsf");
    if(!result)
    {
        cout << result.message << endl;
    }
}

void readTsf()
{
    RootNode r;
    BoolStatus result = fromTsf(r,"example.tsf");
    if(!result)
    {
        cout << result.message << endl;
        return;
    }

    for(DataNode& person : r.items)
    {
        cout << person[_firstName] << " " << person[_lastName] << endl;
        cout << person(_birthday)[_day] << "." << person(_birthday)[_month] << "." << person(_birthday)[_year] << endl;

        if(person.has(_friends) && person(_friends).items.size()>0)
        {
            cout << "Friends:\n";
            for(DataNode& f : person(_friends).items)
            {
                cout << f.value << endl;
            }
        }else{
            cout << "The poor guy has no friends.\n";
        }
        cout << endl;
    }
}

int main()
{
    writeTsf();
    readTsf();
    return 0;
}
