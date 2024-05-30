#ifndef GEO_BATCH_H
#define GEO_BATCH_H

#include "includes.h"

enum class GeoDrawMode
{
    Square,
    Triangle,
    Line,
    None
};

struct geo_batch_vertex
{
    v2f position;
    v4f color;
};

const std::unordered_map<GeoDrawMode, std::function<void(const std::size_t&, std::vector<uint16_t>&)>> indexBuildFunc = 
{
    {
        GeoDrawMode::Square,
        [](const std::size_t& size, std::vector<uint16_t>& indices)
        {
            const std::size_t count = (size >> 2) * 6;
            for (uint16_t i = 0, offset = 0; i < count; i += 6, offset += 4)
            {
                indices.push_back(offset + 0);
                indices.push_back(offset + 1);
                indices.push_back(offset + 3);
                indices.push_back(offset + 0);
                indices.push_back(offset + 3);
                indices.push_back(offset + 2);
            }
        }
    },
    {
        GeoDrawMode::Triangle,
        [](const std::size_t& size, std::vector<uint16_t>& indices)
        {
            for(uint16_t offset = 0; offset < size; offset += 3)
            {
                indices.push_back(offset + 0);
                indices.push_back(offset + 1);
                indices.push_back(offset + 2);
            }
        }
    },
    {
        GeoDrawMode::Line,
        [](const std::size_t& size, std::vector<uint16_t>& indices)
        {
            for(uint16_t offset = 0; offset < size; offset += 2)
            {
                indices.push_back(offset + 0);
                indices.push_back(offset + 1);
            }
        }
    },
    {
        GeoDrawMode::None,
        [](const std::size_t& size, std::vector<uint16_t>& indices)
        {
            return;
        }
    },
};

const std::unordered_map<GeoDrawMode, std::function<void(const std::size_t&)>> renderFuncMap = 
{
    {
        GeoDrawMode::Square,
        [](const std::size_t& size)
        {
            const std::size_t count = (size >> 2) * 6;
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);
        }
    },
    {
        GeoDrawMode::Triangle,
        [](const std::size_t& size)
        {
            glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_SHORT, 0);
        }
    },
    {
        GeoDrawMode::Line,
        [](const std::size_t& size)
        {
            glDrawElements(GL_LINES, size, GL_UNSIGNED_SHORT, 0);
        }
    },
    {
        GeoDrawMode::None,
        [](const std::size_t& size)
        {
            return;
        }
    }
};

struct GeometryBatch
{
    VAO vao;
    Window* window = nullptr;
    Buffer<geo_batch_vertex, GL_ARRAY_BUFFER> vbo;
    Buffer<uint16_t, GL_ELEMENT_ARRAY_BUFFER> ebo;
    GeoDrawMode currDrawMode = GeoDrawMode::None;
    std::vector<geo_batch_vertex> vertices;
    std::vector<uint16_t> indices;
    inline GeometryBatch() = default;
    inline GeometryBatch(Window* window);
    inline void DrawLine(v2f start, v2f end, v4f color);
    inline void DrawRect(v2f pos, v2f size, float rotation, v4f color);
    inline void DrawTriangleOutline(v2f pos1, v2f pos2, v2f pos3, v4f color);
    inline void DrawTriangle(v2f pos1, v2f pos2, v2f pos3, v4f color);
    inline void DrawRectOutline(v2f pos, v2f size, v4f color);
    inline void Flush();
};

#endif

#ifdef GEO_BATCH_H
#undef GEO_BATCH_H

inline GeometryBatch::GeometryBatch(Window* window) : window(window)
{
    vao.Build();
    vbo.Build(GL_DYNAMIC_DRAW);
    ebo.Build(GL_DYNAMIC_DRAW);
    vbo.AddAttrib(0, 2, offsetof(geo_batch_vertex, position));
    vbo.AddAttrib(1, 4, offsetof(geo_batch_vertex, color));
}

inline void GeometryBatch::DrawLine(v2f start, v2f end, v4f color)
{
    assert(window);
    if(currDrawMode != GeoDrawMode::Line || vertices.size() + 2 >= GEO_BATCH_MAX_VERT) this->Flush();
    currDrawMode = GeoDrawMode::Line;
    const v2f scrSize = window->GetScrSize();
    vertices.push_back({
        .position = scrToWorld(
            start,
            scrSize
        ),
        .color = color
    });
    vertices.push_back({
        .position = scrToWorld(
            end,
            scrSize
        ),
        .color = color
    });
}

inline void GeometryBatch::DrawRect(v2f pos, v2f size, float rotation, v4f color)
{
    assert(window);
    if(currDrawMode != GeoDrawMode::Square || vertices.size() + 4 >= GEO_BATCH_MAX_VERT) this->Flush();
    currDrawMode = GeoDrawMode::Square;
    const v2f scrSize = window->GetScrSize();
    v2f outPos;
    size *= 0.5f;
    Transform transform;
    transform.Rotate(rotation);
    transform.Translate(pos.x, pos.y);
    transform.Forward(-size.w, size.h, outPos.x, outPos.y);
    vertices.push_back({
        .position = scrToWorld(
            outPos,
            scrSize
        ),
        .color = color
    });
    transform.Forward(-size.w, -size.h, outPos.x, outPos.y);
    vertices.push_back({
        .position = scrToWorld(
            outPos,
            scrSize
        ),
        .color = color, 
    });
    transform.Forward(size.w, size.h, outPos.x, outPos.y);
    vertices.push_back({
        .position = scrToWorld(
            outPos,
            scrSize
        ),
        .color = color
    });
    transform.Forward(size.w, -size.h, outPos.x, outPos.y);
    vertices.push_back({
        .position = scrToWorld(
            outPos,
            scrSize
        ),
        .color = color 
    });
}

inline void GeometryBatch::DrawTriangle(v2f pos1, v2f pos2, v2f pos3, v4f color)
{
    assert(window);
    if(currDrawMode != GeoDrawMode::Triangle || vertices.size() + 3 >= GEO_BATCH_MAX_VERT) this->Flush();
    currDrawMode = GeoDrawMode::Triangle;
    const v2f scrSize = window->GetScrSize();
    vertices.push_back({
        .position = scrToWorld(
            pos1,
            scrSize
        ),
        .color = color
    });
    vertices.push_back({
        .position = scrToWorld(
            pos2,
            scrSize
        ),
        .color = color, 
    });
    vertices.push_back({
        .position = scrToWorld(
            pos3,
            scrSize
        ),
        .color = color
    });
}

inline void GeometryBatch::DrawRectOutline(v2f pos, v2f size, v4f color)
{
    DrawLine(
    {
        pos.x - size.w * 0.5f,
        pos.y - size.h * 0.5f
    }, 
    {
        pos.x + size.w * 0.5f,
        pos.y - size.h * 0.5f
    }, color);
    DrawLine(
    {
        pos.x - size.w * 0.5f,
        pos.y + size.h * 0.5f
    }, 
    {
        pos.x + size.w * 0.5f,
        pos.y + size.h * 0.5f
    }, color);
    DrawLine( 
    {
        pos.x + size.w * 0.5f,
        pos.y - size.h * 0.5f
    }, 
    {
        pos.x + size.w * 0.5f,
        pos.y + size.h * 0.5f
    }, color);
    DrawLine( 
    {
        pos.x - size.w * 0.5f,
        pos.y - size.h * 0.5f
    }, 
    {
        pos.x - size.w * 0.5f,
        pos.y + size.h * 0.5f
    }, color);
}

inline void GeometryBatch::DrawTriangleOutline(v2f pos1, v2f pos2, v2f pos3, v4f color)
{
    DrawLine(pos1, pos2, color);
    DrawLine(pos2, pos3, color);
    DrawLine(pos3, pos1, color);
}

inline void GeometryBatch::Flush()
{
    assert(window);
    window->SetShader(2);
    const std::size_t size = vertices.size();
    indexBuildFunc.at(currDrawMode)(size, indices);
    vao.Bind();
    vbo.Map(vertices);
    ebo.Map(indices);
    renderFuncMap.at(currDrawMode)(size);
    vao.Unbind();
    indices.clear();
    vertices.clear();
}

#endif