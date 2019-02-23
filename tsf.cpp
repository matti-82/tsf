/*********************************************************************************************************
 *
 * Copyright (c) 2015 Matthias Rossmy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ********************************************************************************************************/

#include <fstream>
#include <map>

#include "tsf.h"

const char NEW_LINE          = '\n';
const char TAB               = '\t';
const char TYPE_SEPARATOR    = '^';
const char ASSIGNMENT        = '=';

const char ESCAPE_CHAR       = '\'';
const char ESCAPE_NEW_LINE   = '|';
const char ESCAPE_TAB        = '>';

const std::string NEW_LINE_STR(1,NEW_LINE);
const std::string TAB_STR(1,TAB);
const std::string TYPE_SEPARATOR_STR(1,TYPE_SEPARATOR);
const std::string ASSIGNMENT_STR(1,ASSIGNMENT);

std::vector<DataNode>::iterator RootNode::find(const std::string& nam, bool autoCreate)
{
    std::vector<DataNode>::iterator it=items.begin();
    for(;it!=items.end();it++)
    {
        if(it->name==nam) return it;
    }
    if(autoCreate)
    {
        items.push_back(DataNode(nam));
        return items.end()-1;
    }else{
        return items.end();
    }
}

DataNode& RootNode::operator()(const std::string& nam)
{
    return *(find(nam, true));
}

bool RootNode::has(const std::string &nam)
{
    return find(nam,false)!=items.end();
}

DataNode* RootNode::add(const std::string &nam, const std::string &typ, const std::string &val)
{
    items.push_back(DataNode(nam,typ,val));
    return &(*(items.end()-1));
}

DataNode::DataNode(const std::string& nam, const std::string& typ, const std::string& val)
{
    name=nam;
    type=typ;
    value=val;
}

std::string& DataNode::operator[](const std::string& nam)
{
    return find(nam, true)->value;
}

void DataNode::set(const std::string &nam, const std::string &val)
{
    (*this)[nam] = val;
}

BoolStatus::BoolStatus(bool succ, const std::string &msg)
{
    success = succ;
    message = msg;
}

std::string escapeTsf(const std::string& s)
{
    std::string result;
    for(size_t x=0; x<s.length(); x++)
    {
        char c = s[x];

        if(c==ESCAPE_CHAR)
        {
            result += ESCAPE_CHAR;
            result += ESCAPE_CHAR;
        }
        else if(c==NEW_LINE)
        {
            result += ESCAPE_CHAR;
            result += ESCAPE_NEW_LINE;
        }
        else if(c==TAB)
        {
            result += ESCAPE_CHAR;
            result += ESCAPE_TAB;
        }
        else if(c==ASSIGNMENT)
        {
            result += ESCAPE_CHAR;
            result += ASSIGNMENT;
        }
        else if(c==TYPE_SEPARATOR)
        {
            result += ESCAPE_CHAR;
            result += TYPE_SEPARATOR;
        }
        else
        {
            result += c;
        }
    }
    return result;
}

BoolStatus toTsfHelper(DataNode& node, std::ostream& dest, int level)
{
    for(int x=0; x<level; x++) dest << TAB_STR;

    if(node.name.length()>0)
    {
        if(node.name[0]==' ') return BoolStatus(false,"Node names are not allowed to start with space.");
        dest << escapeTsf(node.name);
    }
    if(node.type.length()>0)
    {
        dest << TYPE_SEPARATOR_STR << escapeTsf(node.type);
    }
    if((node.value.length()>0 && (node.name.length()>0 || node.type.length()>0)) //es müssen name/type und value getrennt werden
            || (node.name.length()>0 && node.type.length()==0) //es wäre sonst Verwechselungsgefahr mit einem alleinstehenden Wert
            || (node.name.length()==0 && node.type.length()==0 && node.value.length()==0)) //die Zeile würde sonst leer sein
    {
        dest << ASSIGNMENT_STR;
    }
    if(node.value.length()>0)
    {
        dest << escapeTsf(node.value);
    }
    dest << NEW_LINE_STR;

    return toTsf(node,dest,level+1);
}

BoolStatus toTsf(RootNode& node, std::ostream& dest, int level)
{
    for(std::vector<DataNode>::iterator it=node.items.begin(); it!=node.items.end(); it++)
    {
        BoolStatus result = toTsfHelper(*it,dest,level);
        if(!result) return result;
    }
    return BoolStatus(true);
}

BoolStatus toTsf(RootNode &node, const std::string &destFile)
{
    BoolStatus result;
    try
    {
        std::ofstream outfile;
        outfile.open(destFile.c_str());
        result = toTsf(node,outfile);
        outfile.close();
    }
    catch(...)
    {
        return BoolStatus(false, "Exception in writing output file.");
    }
    return result;
}

BoolStatus fromTsf(RootNode& dest, std::istream& source)
{
    dest.items.clear();

    std::map<int,RootNode*> parents;
    parents[0] = &dest;
    size_t maxAllowedLevel = 0;

    std::string line;
    while(!source.eof())
    {
        if(!source.good()) return BoolStatus(false, "Error in reading input file.");

        std::getline(source,line);
        if(line.length()==0) continue;

        size_t level=0;
        while(line[level]==TAB) level++;
        if(level>maxAllowedLevel) return BoolStatus(false, "A line can have at most 1 tab indentation more than the previous line.");
        maxAllowedLevel = level+1;

        if(line[level]==' ') return BoolStatus(false, "Node names are not allowed to start with space. Maybe you used spaces instead of tabs for indentation.");

        auto item = parents[level]->add();
        parents[level+1] = item;

        bool hasTypeSep = false;
        bool hasAssign = false;

        std::string* s = &(item->name);
        for(size_t pos=level; pos<line.length(); pos++)
        {
            char c = line[pos];
            if(c==ESCAPE_CHAR)
            {
                char c = line[++pos];
                if(c==ESCAPE_NEW_LINE)
                {
                    (*s) += NEW_LINE;
                }
                else if(c==ESCAPE_TAB)
                {
                    (*s) += TAB;
                }
                else
                {
                    (*s) += c;
                }
            }
            else if(c==TYPE_SEPARATOR)
            {
                if(hasTypeSep || hasAssign) return BoolStatus(false, "Only 1 type indicator is allowed per line and no type indicator is allowed after assignment.");
                s = &(item->type);
                hasTypeSep = true;
            }
            else if(c==ASSIGNMENT)
            {
                if(hasAssign) return BoolStatus(false,"Only 1 assignment is allowed per line.");
                s = &(item->value);
                hasAssign = true;
            }
            else
            {
                (*s) += c;
            }
        }

        if(!hasTypeSep && !hasAssign)
        {
            item->value = item->name;
            item->name = "";
            item->type = "";
        }
    }
    return BoolStatus(true);
}

BoolStatus fromTsf(RootNode &dest, const std::string &sourceFile)
{
    BoolStatus result;
    try
    {
        std::ifstream infile;
        infile.open(sourceFile.c_str(), std::ifstream::in);
        result = fromTsf(dest,infile);
        infile.close();
    }
    catch(...)
    {
        return BoolStatus(false, "Exception in reading input file.");
    }
    return result;
}

