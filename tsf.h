/*********************************************************************************************************
 *
 * Copyright (c) 2015-2019 Matthias Rossmy
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

//version 2.0

#pragma once

#include <string>
#include <vector>

// Data Structure //////////////////////////////////////

class DataNode;

class RootNode
{
public:
    //list of all sub-items
    std::vector<DataNode> items;

    //get a sub-item by name as iterator
    std::vector<DataNode>::iterator find(const std::string& nam, bool autoCreate=false);

    //get a sub-item by name as reference
    DataNode& operator()(const std::string& nam);

    //check if a sub-item exists
    bool has(const std::string& nam);

    //add a sub-item
    DataNode* add(const std::string& nam="", const std::string& typ="", const std::string& val="");
};

class DataNode : public RootNode
{
public:
    DataNode(const std::string& nam="", const std::string& typ="", const std::string& val="");

    std::string name;
    std::string type;
    std::string value;

    //get or set the value of a sub-item
    std::string& operator[](const std::string& nam);

    //set the value of a sub-item (this function is more handy when having DataNode pointers)
    void set(const std::string& nam, const std::string& val);
};

// Helper Stuff ////////////////////////////////////////

class BoolStatus
{
public:
    BoolStatus(bool succ=false, const std::string& msg="");
    std::string message;
    bool success;
    operator bool() const { return success; }
};

//#define ID_STR(x) namespace id { std::string x(#x); }
#define ID_STR(x) std::string _##x(#x)

// (De)Serialization Functions ///////////////////////////

BoolStatus toTsf(RootNode& node, const std::string& destFile);
BoolStatus toTsf(RootNode& node, std::ostream& dest, int level=0);

BoolStatus fromTsf(RootNode& dest, const std::string& sourceFile);
BoolStatus fromTsf(RootNode& dest, std::istream& source);

