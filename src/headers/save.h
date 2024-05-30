#ifndef SAVE_H
#define SAVE_H

#include "includes.h"

const std::string whitespaces = " \n\t\v\0";
const std::string seperator = "->";

template <typename T> inline std::string convert(const T& value) {return std::to_string(value);}
template <> inline std::string convert<bool>(const bool& value) {return value ? "true" : "false";}
template <typename T> inline std::optional<T> convert(const std::optional<std::string> str) {}
template <> inline std::optional<double> convert<double>(const std::optional<std::string> str) 
{return str.has_value() ? std::make_optional(std::stod(str.value().c_str())) : std::nullopt;}
template <> inline std::optional<float> convert<float>(const std::optional<std::string> str) 
{return str.has_value() ? std::make_optional(std::stof(str.value().c_str())) : std::nullopt;}
template <> inline std::optional<int> convert<int>(const std::optional<std::string> str) 
{return str.has_value() ? std::make_optional(std::stoi(str.value().c_str())) : std::nullopt;}
template <> inline std::optional<bool> convert<bool>(const std::optional<std::string> str)
{
    if(str.has_value())
    {
        if(str.value() == "true") return true;
        else if(str.value() == "false") return false;
        else return std::nullopt;
    }
    return std::nullopt;
}

struct Container
{
    std::string content;
    std::optional<std::string> name = std::nullopt;
    template <typename T> inline std::optional<T> get()
    {
        return convert<T>(content);
    }
};

inline std::vector<std::string> ParseDirectory(const std::string& dir)
{
    std::vector<std::string> directory;
    std::string buffer;
    auto ClearBuffer = [&]()
    {   
        directory.push_back(buffer);
        buffer.clear();
    };
    std::size_t index = 0, next = dir.find_first_of(seperator, index);
    while(index < dir.size() && next != std::string::npos)
    {
        directory.push_back(dir.substr(index, next - index));
        index = next + seperator.size();
        next = dir.find_first_of(seperator, index);
    }
    if(next == std::string::npos)
        directory.push_back(dir.substr(index, dir.size() - index));
    return directory;    
};

template <class T> struct allowed_id_type :
    std::integral_constant<bool, 
        std::is_integral<T>::value ||
        std::is_convertible<T, std::string>::value>
    {};

template <class T> struct allowed_data_type : 
    std::integral_constant<bool,
        std::is_arithmetic<T>::value ||
        std::is_same<T, bool>::value>
    {};

struct DataNode
{
    DataNode() = default;
    template <class Data, class ID> inline void SetData(const Data& data, ID id)
    {
        static_assert(allowed_id_type<ID>::value && allowed_data_type<Data>::value);
        SetString(convert<Data>(data), id);
    }
    template <class ID> inline void Rename(const std::string& name, ID id)
    {
        static_assert(allowed_id_type<ID>::value);
        auto container = FindContainer(id);
        if(container.has_value()) container.value().get().name = name;
    }
    inline std::optional<std::reference_wrapper<DataNode>> GetProperty(const std::string& dir);
    inline void SetString(const std::string& str, const std::size_t& index = 0);
    inline void SetString(const std::string& str, const std::string& name = "");
    inline std::optional<std::string> GetName(const std::size_t& index = 0);
    inline bool HasProperty(const std::string& dir);
    inline void data_foreach(std::function<void(Container)> f);
    inline void data_indexed_for(std::function<void(Container, std::size_t index)> f);
    inline void nodes_foreach(std::function<void(std::pair<std::string, DataNode>)> f);
    inline void nodes_indexed_for(std::function<void(std::pair<std::string, DataNode>, std::size_t)> f);
    inline std::optional<std::reference_wrapper<Container>> FindContainer(const std::size_t& index = 0);
    inline std::optional<std::reference_wrapper<Container>> FindContainer(const std::string& name = "");
    inline void SetData(const std::string& str);
    inline const std::string GetData() const;
    inline void clear();
    inline const DataNode& at(const std::string& str) const;
    inline DataNode& operator[](const std::string& str);
    std::vector<Container> data;
    std::unordered_map<std::string, DataNode> nodes;
};

inline void Serialize(DataNode& node, const std::string& file)
{
    std::ofstream output(file.c_str(), std::ios::trunc);
    int tabCount = 0;
    auto Indent = [](int count)->std::string
    {
        std::string res;
        for(int index = 0; index < count; index++) res += '\t';
        return res;
    };
    auto AddBrackets = [](std::string str)->std::string
    {
        if(str.find(whitespaces.c_str(), 0, 1) != std::string::npos) return '[' + str + ']';
        else return str;
    };
    auto Write = [&](std::pair<std::string, DataNode> p) -> void
    {
        auto WriteNode = [&](std::pair<std::string, DataNode> p, auto& WriteRef) mutable -> void
        {
            const std::string name = AddBrackets(p.first) + '>';
            const std::string data = '{' + AddBrackets(p.second.GetData()) + '}';
            if(!p.second.nodes.empty())
            {
                output << Indent(tabCount++) << '<' << name << '\n';
                if(!p.second.data.empty()) output << Indent(tabCount) << data << '\n';
                for(auto& node : p.second.nodes) WriteRef(node, WriteRef);
                output << Indent(--tabCount) << "</" << name << '\n';
            }
            else
            {
                output << Indent(tabCount) << '<' << name << data << "</" << name << '\n';
            }
        };
        WriteNode(p, WriteNode);
    };
    if(!node.data.empty()) output << Indent(tabCount) << '{' << AddBrackets(node.GetData()) << '}' << '\n';
    for(auto& p : node.nodes) Write(p);
    output.close();
}

inline void Deserialize(std::reference_wrapper<DataNode> node, const std::string& path)
{
    node.get().clear();
    std::stack<std::pair<std::reference_wrapper<DataNode>, std::string>> nodeStack;
    std::ifstream file(path.c_str());
    bool openBracket = false;
    std::string buffer;
    char c;

    auto TrimWhitespaces = [](const std::string& str) -> std::string
    {
        std::string res;
        int openBracketCount = 0;
        for(std::size_t index = 0; index < str.size(); index++)
            switch(str.at(index))
            {
                case '[':
                {
                    openBracketCount++;
                }
                break;
                case ']':
                {
                    openBracketCount--;
                } 
                break;
                default: 
                {
                    if(whitespaces.find(str.at(index)) != std::string::npos && openBracketCount == 0);
                    else res += str.at(index);
                }
                break;
            }
        return res;
    };

    auto ParseData = [&]()
    {
        nodeStack.top().first.get().SetData(buffer);
        buffer.clear();
    };

    auto ParseObject = [&]()
    {
        bool close = buffer.front() == '/';
        if(close)
        {
            buffer.erase(buffer.begin());
            if(nodeStack.top().second == buffer) nodeStack.pop();
        }
        else
        {
            std::reference_wrapper<DataNode> newNode = nodeStack.empty() ? node.get()[buffer] : nodeStack.top().first.get()[buffer];
            nodeStack.push(std::make_pair(newNode, buffer));
        }
        buffer.clear();
    };

    while(file.get(c))
    {
        if(!openBracket)
        {
            openBracket = (c == '<' || c == '{');
        }
        else if(c == '>')
        {
            buffer = TrimWhitespaces(buffer);
            ParseObject();
            openBracket = false;
        }
        else if(c == '}')
        {
            buffer = TrimWhitespaces(buffer);
            ParseData();
            openBracket = false;
        }
        else
            buffer.push_back(c);
    }

    file.close();
}

template <class ID> inline std::optional<std::string> GetString(std::optional<DataNode> node, ID id)
{
    static_assert(allowed_id_type<ID>::value);
    if(node.has_value())
    {
        auto container = node.value().FindContainer(id);
        return container.has_value() ? std::make_optional(container.value().get().content) : std::nullopt;
    }
    return std::nullopt;
}

template <class Data, class ID> inline std::optional<Data> GetData(std::optional<DataNode> node, ID id)
{
    static_assert(allowed_id_type<ID>::value && allowed_data_type<Data>::value);
    return convert<Data>(GetString(node, id));
}

#endif

#ifdef SAVE_H
#undef SAVE_H


inline std::optional<std::reference_wrapper<Container>> DataNode::FindContainer(const std::size_t& index)
{
#if defined NO_COLLISIONS
    for(std::size_t i = 0, count = 0; i < data.size(); i++, count += data[i].name.has_value() ? 0 : 1)
        if(count == index)
            return data[i];
    return std::nullopt;
#else
    if(index < data.size()) return data[index];
    else return std::nullopt;
#endif
}

inline std::optional<std::reference_wrapper<Container>> DataNode::FindContainer(const std::string& name)
{
    for(auto& element : data)
        if(element.name.has_value() && element.name.value() == name)
            return element;
    return std::nullopt;
}

inline void DataNode::SetString(const std::string& str, const std::size_t& index)
{
    auto container = FindContainer(index);
    if(container.has_value()) container.value().get().content = str;
}

inline void DataNode::SetString(const std::string& str, const std::string& name)
{
    auto container = FindContainer(name);
    if(container.has_value())
    {
        container.value().get().content = str;
        return;
    }
    data.push_back(Container{str, name.empty() ? std::nullopt : std::make_optional(name)});
}

inline std::optional<std::string> DataNode::GetName(const std::size_t& index)
{
    auto container = FindContainer(index);
    return container.has_value() ? container.value().get().name : std::nullopt;
}

DataNode& DataNode::operator[](const std::string& str)
{
    if(nodes.count(str) == 0) nodes[str] = DataNode();
    return nodes[str];
}

inline void DataNode::nodes_foreach(std::function<void(std::pair<std::string, DataNode>)> f)
{
    for(auto& node : nodes) f(node);
}

inline void DataNode::data_foreach(std::function<void(Container)> f)
{
    for(auto& c : data) f(c);
}

inline void DataNode::nodes_indexed_for(std::function<void(std::pair<std::string, DataNode>, std::size_t)> f)
{
    std::size_t index = 0;
    for(auto iter = nodes.begin(); iter != nodes.end(); iter++)
        f(*iter, index++);
}

inline void DataNode::data_indexed_for(std::function<void(Container, std::size_t index)> f)
{
    for(std::size_t index = 0; index < data.size(); index++)
        f(data[index], index);
}

inline std::optional<std::reference_wrapper<DataNode>> DataNode::GetProperty(const std::string& directory)
{
    std::reference_wrapper<DataNode> res = *this;
    for(auto& element : ParseDirectory(directory))
    {
        if(res.get().nodes.count(element) != 0)
            res = res.get()[element];
        else
            return std::nullopt;
    }
    return res;
}

inline bool DataNode::HasProperty(const std::string& directory)
{
    std::reference_wrapper<DataNode> node = *this;
    for(auto& element : ParseDirectory(directory))
    {
        if(node.get().nodes.count(element) == 0)
            return false;
        else
            node = node.get()[element];
    }
    return true;
}

inline void DataNode::clear()
{
    data.clear();
    for(auto& node : nodes) node.second.clear();
    nodes.clear();
}

inline const DataNode& DataNode::at(const std::string& str) const
{
    return nodes.at(str);
}

inline void DataNode::SetData(const std::string& str)
{
    data.clear();
    auto ClearBuffer = [&](const std::string& buffer)
    {
        Container container;
        for(std::size_t index = 0; index < buffer.size(); index++)
        {
            if(buffer.at(index) == '(')
            {
                const std::size_t end = buffer.find_first_of(')', ++index);
                container.name = buffer.substr(index, end - index);
                index = end;
            }
            else
                container.content += buffer.at(index);
        }
        data.push_back(container);
    };
    std::size_t index = 0, next = str.find_first_of(',', index);
    while(index < str.size() && next != std::string::npos)
    {
        ClearBuffer(str.substr(index, next - index));
        index = next;
        next = str.find_first_of(',', ++index);
    }
    if(next == std::string::npos)
        ClearBuffer(str.substr(index, str.size() - index));    
}

inline const std::string DataNode::GetData() const
{
    std::string res;
    std::size_t index = 0;
    while(index < data.size()) 
    {
        res += data.at(index).name.has_value() ? '(' + data.at(index).name.value() + ')' : "";
        res += data.at(index).content;
        res += ++index < data.size() ? "," : "";
    }
    return res;
}

#endif