#ifndef BUFFER_H
#define BUFFER_H

#include "includes.h"

struct VAO
{
    inline VAO(const VAO& lhs) = delete;
    inline VAO& operator=(const VAO& lhs) = delete;
    inline VAO()
    {
        id = 0;
    }
    inline VAO(VAO&& lhs) : id(lhs.id)
    {
        lhs.id = 0;
    }
    inline VAO& operator=(VAO&& lhs)
    {
        if(this != &lhs)
        {
            Release();
            id = lhs.id;
            lhs.id = 0;
        }
        return *this;
    }
    inline void Build()
    {
        glGenVertexArrays(1, &id);
        glBindVertexArray(id);
    }
    inline void Bind()
    {
        if(!id)
        {
            Build();
        }
        glBindVertexArray(id);
    }
    inline void Unbind()
    {
        glBindVertexArray(0);
    }
    inline void Release()
    {
        if(id) glDeleteVertexArrays(1, &id);
    }
    inline virtual ~VAO() 
    {
        Release();
    }
private:
    GLuint id;
};

template <class T, GLenum type> struct Buffer
{
    inline Buffer(const Buffer<T, type>& lhs) = delete;
    inline Buffer& operator=(const Buffer<T, type>& lhs) = delete;
    inline Buffer(Buffer<T, type>&& lhs) : id(lhs.id), mapFlag(lhs.mapFlag), size(lhs.size)
    {
        lhs.id = 0;
        lhs.size = 0;
    }
    inline Buffer(const std::size_t& size = 0, const GLenum& mapFlag = GL_STATIC_DRAW) : mapFlag(mapFlag), size(size)
    {
        id = 0;
    }
    inline Buffer& operator=(Buffer<T, type>&& lhs)
    {
        if(this != &lhs)
        {
            Release();
            id = lhs.id;
            size = lhs.size;
            mapFlag = lhs.mapFlag;
            lhs.id = 0;
            lhs.size = 0;
        }
        return *this;
    }
    inline void Build(const T* data, const std::size_t& numElements, const GLenum& flag = GL_STATIC_DRAW)
    {
        *this = Buffer(numElements, flag);
        glGenBuffers(1, &id);
        glBindBuffer(type, id);
        glBufferData(type, size * sizeof(T), data, mapFlag);
    }
    inline void Build(const std::vector<T>& vec, const GLenum& flag = GL_STATIC_DRAW)
    {
        Build(vec.data(), vec.size(), flag);
    }
    inline void Build(const GLenum& flag = GL_STATIC_DRAW)
    {
        Build(NULL, 0, flag);
    }
    inline void Map(const T* data, const std::size_t& numElements, const std::size_t& offset = 0)
    {
        if(!id)
        {
            Build(data, numElements, mapFlag);
            return;
        }
        glBindBuffer(type, id);
        if(size < numElements)
        {
            glBufferData(type, sizeof(T) * numElements, data, mapFlag);
            size = numElements;
        } 
        else
            glBufferSubData(type, sizeof(T) * offset, sizeof(T) * numElements, data);
    }
    inline void Map(
        typename std::vector<T>::iterator& beg,
        typename std::vector<T>::iterator& end,
        const std::size_t& offset = 0)
    {
        Map(&(*beg), end - beg, offset);
    }
    inline void Map(const std::vector<T>& vec, const std::size_t& offset = 0)
    {
        Map(vec.data(), vec.size(), offset);
    }
    template <GLenum buffType = type, typename = typename std::enable_if<buffType == GL_ARRAY_BUFFER>::type>
    inline void AddAttrib(
        const std::size_t& index,
        const std::size_t& numElements,
        const std::size_t& offset,
        const GLenum& attribDataType = GL_FLOAT)
    {
        if(!id)
        {
            Build(mapFlag);
        }
        glVertexAttribPointer(index, numElements, attribDataType, GL_FALSE, sizeof(T), (void*)offset);
        glEnableVertexAttribArray(index);
    }
    inline void Clear()
    {
        if(size != 0)
        {
            glBindBuffer(type, id);
            glBufferSubData(type, 0, size * sizeof(T), NULL);
        }
    }
    inline void Resize(const std::size_t& lhs)
    {
        if(size != lhs)
        {
            size = lhs;
            glBindBuffer(type, id);
            glBufferData(type, size * sizeof(T), NULL, mapFlag);
        }
    }
    inline void Bind()
    {
        if(!id)
        {
            Build(mapFlag);
        }
        glBindBuffer(type, id);
    }
    inline void Unbind()
    {
        glBindBuffer(type, 0);
    }
    inline void Release()
    {
        if(id) glDeleteBuffers(1, &id);
    }
    inline virtual ~Buffer() 
    {
        Release();
    }
private:
    GLuint id;
    GLenum mapFlag;
    std::size_t size;
};

#endif