
//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.3
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
#ifndef AGG_PATH_STORAGE_INCLUDED
#define AGG_PATH_STORAGE_INCLUDED
#include "agg_basics.h"
namespace agg
{
class path_storage 
{
    enum block_scale_e {
        block_shift = 8,
        block_size  = 1 << block_shift,
        block_mask  = block_size - 1,
        block_pool  = 256
    };
public:
    class vertex_source 
    {
    public:
        vertex_source() {}
        vertex_source(const path_storage& p) : m_path(&p), m_vertex_idx(0) {}
        void rewind(unsigned path_id)
        {
            m_vertex_idx = path_id;
        }
        unsigned vertex(float* x, float* y)
        {
          return (m_vertex_idx < m_path->total_vertices())
                     ? m_path->vertex(m_vertex_idx++, x, y)
                     : static_cast<unsigned>(path_cmd_stop);
        }
    private:
        const path_storage* m_path;
        unsigned            m_vertex_idx;
    };
    ~path_storage();
    path_storage();
    unsigned last_vertex(float* x, float* y) const;
    unsigned prev_vertex(float* x, float* y) const;
    void move_to(float x, float y);
    void line_to(float x, float y);
    void curve4(float x_ctrl1, float y_ctrl1,
                float x_ctrl2, float y_ctrl2,
                float x_to,    float y_to);
    template<class VertexSource>
    void add_path(VertexSource& vs,
                  unsigned path_id = 0,
                  bool solid_path = true)
    {
        float x, y;
        unsigned cmd;
        vs.rewind(path_id);
        while(!is_stop(cmd = vs.vertex(&x, &y))) {
            if(is_move_to(cmd) && solid_path && m_total_vertices) {
                cmd = path_cmd_line_to;
            }
            add_vertex(x, y, cmd);
        }
    }
    template<class VertexSource>
    void add_path_curve(VertexSource& vs,
                        unsigned path_id = 0,
                        bool solid_path = true)
    {
        float x, y;
        unsigned cmd;
        int flag;
        vs.rewind(path_id);
        while(!is_stop(cmd = vs.vertex_curve_flag(&x, &y, flag))) {
            if(is_move_to(cmd) && solid_path && m_total_vertices) {
                cmd = path_cmd_line_to | flag;
            }
            add_vertex(x, y, cmd | flag);
        }
    }
    unsigned total_vertices() const
    {
        return m_total_vertices;
    }
    unsigned vertex(unsigned idx, float* x, float* y) const
    {
        unsigned nb = idx >> block_shift;
        const float* pv = m_coord_blocks[nb] + ((idx & block_mask) << 1);
        *x = *pv++;
        *y = *pv;
        return m_cmd_blocks[nb][idx & block_mask];
    }
    unsigned command(unsigned idx) const
    {
        return m_cmd_blocks[idx >> block_shift][idx & block_mask];
    }
    unsigned getflag(unsigned idx) const
    {
        return m_cmd_blocks[idx >> block_shift][idx & block_mask] & path_flags_jr;
    }
    void     rewind(unsigned path_id);
    unsigned vertex(float* x, float* y);
    void add_vertex(float x, float y, unsigned cmd);
    void end_poly();
private:
    void allocate_block(unsigned nb);
    unsigned char* storage_ptrs(float** xy_ptr);
private:
    unsigned        m_total_vertices;
    unsigned        m_total_blocks;
    unsigned        m_max_blocks;
    float**   m_coord_blocks;
    unsigned char** m_cmd_blocks;
    unsigned        m_iterator;
};
inline unsigned path_storage::vertex(float* x, float* y)
{
    if(m_iterator >= m_total_vertices) {
        return path_cmd_stop;
    }
    return vertex(m_iterator++, x, y);
}
inline unsigned path_storage::prev_vertex(float* x, float* y) const
{
    if(m_total_vertices > 1) {
        return vertex(m_total_vertices - 2, x, y);
    }
    return path_cmd_stop;
}
inline unsigned path_storage::last_vertex(float* x, float* y) const
{
    if(m_total_vertices) {
        return vertex(m_total_vertices - 1, x, y);
    }
    return path_cmd_stop;
}
inline unsigned char* path_storage::storage_ptrs(float** xy_ptr)
{
    unsigned nb = m_total_vertices >> block_shift;
    if(nb >= m_total_blocks) {
        allocate_block(nb);
    }
    *xy_ptr = m_coord_blocks[nb] + ((m_total_vertices & block_mask) << 1);
    return m_cmd_blocks[nb] + (m_total_vertices & block_mask);
}
inline void path_storage::add_vertex(float x, float y, unsigned cmd)
{
    float* coord_ptr = 0;
    unsigned char* cmd_ptr = storage_ptrs(&coord_ptr);
    *cmd_ptr = (unsigned char)cmd;
    *coord_ptr++ = x;
    *coord_ptr   = y;
    m_total_vertices++;
}
inline void path_storage::move_to(float x, float y)
{
    add_vertex(x, y, path_cmd_move_to);
}
inline void path_storage::line_to(float x, float y)
{
    add_vertex(x, y, path_cmd_line_to);
}
}
#endif
