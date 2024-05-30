#ifndef SPRITE_BATCH_H
#define SPRITE_BATCH_H

#include "includes.h"

struct sprite_batch_vertex
{
    v2f position;
    v2f texcoord;
    v4f color;
    GLuint texture;
};

struct SpriteBatch
{
    VAO vao;
    Window* window = nullptr;
    Buffer<sprite_batch_vertex, GL_ARRAY_BUFFER> vbo;
    Buffer<uint16_t, GL_ELEMENT_ARRAY_BUFFER> ebo;
    std::vector<sprite_batch_vertex> vertices;
    std::vector<uint16_t> indices;
    std::vector<GLuint> textures;
    inline SpriteBatch() = default;
    inline SpriteBatch(Window* window);
    inline void Draw(
        const Decal& dec,
        const int32_t x,
        const int32_t y,
        const v2f& size = 1.0f,
        const float rotation = 0.0f,
        Horizontal hor = Horizontal::Norm,
        Vertical ver = Vertical::Norm,
        const v4f& color = 1.0f,
        rect src = {0.0f, 0.0f, 1.0f, 1.0f}
    );
    inline void Draw(
        const Decal& dec,
        Transform& transform,
        Horizontal hor = Horizontal::Norm,
        Vertical ver = Vertical::Norm,
        const v4f& color = 1.0f,
        rect src = {0.0f, 0.0f, 1.0f, 1.0f}
    );
    inline void Draw(
        const Decal& dec,
        const rect& dst,
        Horizontal hor = Horizontal::Norm,
        Vertical ver = Vertical::Norm,
        const v4f& color = 1.0f,
        rect src = {0.0f, 0.0f, 1.0f, 1.0f}
    );
    inline void Flush();
};

#endif

#ifdef SPRITE_BATCH_H
#undef SPRITE_BATCH_H


inline SpriteBatch::SpriteBatch(Window* window) : window(window)
{
    vao.Build();
    vbo.Build(GL_DYNAMIC_DRAW);
    ebo.Build(GL_DYNAMIC_DRAW);
    vbo.AddAttrib(0, 2, offsetof(sprite_batch_vertex, position));
    vbo.AddAttrib(1, 2, offsetof(sprite_batch_vertex, texcoord));
    vbo.AddAttrib(2, 4, offsetof(sprite_batch_vertex, color));
    vbo.AddAttrib(3, 1, offsetof(sprite_batch_vertex, texture));
}

inline void SpriteBatch::Draw(
    const Decal& dec, 
    Transform& transform,
    Horizontal hor,
    Vertical ver,
    const v4f& color, 
    rect src)
{
    assert(window);
    v2f outPos;
    if(hor == Horizontal::Flip) std::swap(src.sx, src.ex);
    if(ver == Vertical::Flip) std::swap(src.sy, src.ey);
    const GLuint tex = textures.size() % MAX_SPRITES;
    const v2f scrSize = window->GetScrSize();
    const float dw = dec.width;
    const float dh = dec.height;
    transform.Forward(-dw, dh, outPos.x, outPos.y);
    vertices.push_back({
        .position = scrToWorld(
            outPos,
            scrSize
        ),
        .texcoord = {
            src.sx,
            src.ey
        },
        .color = color,
        .texture = tex
    });
    transform.Forward(-dw, -dh, outPos.x, outPos.y);
    vertices.push_back({
        .position = scrToWorld(
            outPos,
            scrSize
        ),
        .texcoord = {
            src.sx,
            src.sy
        },
        .color = color, 
        .texture = tex
    });
    transform.Forward(dw, dh, outPos.x, outPos.y);
    vertices.push_back({
        .position = scrToWorld(
            outPos,
            scrSize
        ),
        .texcoord = {
            src.ex,
            src.ey
        },
        .color = color,
        .texture = tex
    });
    transform.Forward(dw, -dh, outPos.x, outPos.y);
    vertices.push_back({
        .position = scrToWorld(
            outPos,
            scrSize
        ),
        .texcoord = {
            src.ex,
            src.sy
        },
        .color = color,
        .texture = tex
    });
    textures.push_back(dec.id);
}

inline void SpriteBatch::Draw(
    const Decal& dec, 
    const rect& dst,
    Horizontal hor,
    Vertical ver,
    const v4f& color, 
    rect src)
{
    assert(window);
    if(hor == Horizontal::Flip) std::swap(src.sx, src.ex);
    if(ver == Vertical::Flip) std::swap(src.sy, src.ey);
    const GLuint tex = textures.size() % MAX_SPRITES;
    const v2f scrSize = window->GetScrSize();
    vertices.push_back({
        .position = scrToWorld(
            {dst.sx, dst.ey},
            scrSize
        ),
        .texcoord = {
            src.sx,
            src.ey
        },
        .color = color,
        .texture = tex
    });
    vertices.push_back({
        .position = scrToWorld(
            {dst.sx, dst.sy},
            scrSize
        ),
        .texcoord = {
            src.sx,
            src.sy
        },
        .color = color, 
        .texture = tex
    });
    vertices.push_back({
        .position = scrToWorld(
            {dst.ex, dst.ey},
            scrSize
        ),
        .texcoord = {
            src.ex,
            src.ey
        },
        .color = color,
        .texture = tex
    });
    vertices.push_back({
        .position = scrToWorld(
            {dst.ex, dst.sy},
            scrSize
        ),
        .texcoord = {
            src.ex,
            src.sy
        },
        .color = color,
        .texture = tex
    });
    textures.push_back(dec.id);
}

inline void SpriteBatch::Draw(
    const Decal& dec, 
    const int32_t x, 
    const int32_t y, 
    const v2f& size, 
    const float rotation,
    Horizontal hor,
    Vertical ver,
    const v4f& color, 
    rect src)
{
    assert(window);
    Transform transform;
    transform.Rotate(rotation);
    transform.Translate(x, y);
    transform.Scale(size.w, size.h);
    this->Draw(dec, transform, hor, ver, color, src);
}

inline void SpriteBatch::Flush()
{
    assert(window);
    window->SetShader(1);
    int lastTextureIndex = 0;
    auto vertBegin = vertices.begin();
    auto GenIndices = [&](const std::size_t& count)
    {
        indices.resize(count);
        for (uint16_t i = 0, offset = 0; i < count; i += 6, offset += 4)
        {
            indices[i + 0] = offset + 0;
            indices[i + 1] = offset + 1;
            indices[i + 2] = offset + 3;
            indices[i + 3] = offset + 0;
            indices[i + 4] = offset + 3;
            indices[i + 5] = offset + 2;
        }
    };
    auto BindTextures = [&](const std::size_t& spriteCount)
    {
        for(int i = 0; i < spriteCount; i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[lastTextureIndex + i]);
        }
        lastTextureIndex += spriteCount;
    };
    auto DrawBatch = [&](const std::size_t& spriteCount)
    {
        vbo.Resize(spriteCount * 4);
        vbo.Map(&(*vertBegin), spriteCount * 4);
        GenIndices(spriteCount * 6);
        ebo.Map(indices);
        BindTextures(spriteCount);
        glDrawElements(GL_TRIANGLES, spriteCount * 6, GL_UNSIGNED_SHORT, 0);
        vertBegin += spriteCount * 4;
    };
    vao.Bind();
    while(vertBegin != vertices.end())
    {
        const std::size_t size = (vertices.end() - vertBegin) >> 2;
        DrawBatch(size < MAX_SPRITES ? size : MAX_SPRITES);
    }
    vao.Unbind();
    textures.clear();
    vertices.clear();
    indices.clear();
}

#endif