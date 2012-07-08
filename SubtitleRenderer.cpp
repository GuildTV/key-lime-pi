/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "SubtitleRenderer.h"
#include "folly/Unicode.h"
#include "folly/ScopeGuard.h"
#include "enforce.h"

#include <VG/vgu.h>
#include <cassert>

class BoxRenderer {
  VGPath path_;
  VGPaint paint_;

public:
  BoxRenderer(unsigned int opacity) {
    path_ = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F,
                        1.0, 0.0,
                        0, 0,
                        VG_PATH_CAPABILITY_ALL);
    assert(path_);

    paint_ = vgCreatePaint();
    assert(paint_);

    vgSetColor(paint_, opacity);
    assert(!vgGetError());
  }

  ~BoxRenderer() {
    vgDestroyPath(path_);
    assert(!vgGetError());
    vgDestroyPaint(paint_);
    assert(!vgGetError());
  }

  void clear() {
    vgClearPath(path_, VG_PATH_CAPABILITY_ALL);
    assert(!vgGetError());
  }

  void push(int x, int y, int width, int height) {
    assert(width >= 0);
    assert(height >= 0);

    vguRect(path_, x, y, width, height);
    assert(!vgGetError());
  };

  void render() {
    vgSetPaint(paint_, VG_FILL_PATH);
    assert(!vgGetError());

    vgDrawPath(path_, VG_FILL_PATH);
    assert(!vgGetError());
  }
};

void SubtitleRenderer::load_glyph(char32_t codepoint) {
  VGfloat escapement[2]{};

  auto load_glyph_internal =
  [&,this](FT_Face ft_face, VGFont vg_font, bool border) {
    try {
      auto glyph_index = FT_Get_Char_Index(ft_face, codepoint);
      ENFORCE(!FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_NO_HINTING));

      FT_Glyph glyph;
      ENFORCE(!FT_Get_Glyph(ft_face->glyph, &glyph));
      SCOPE_EXIT {FT_Done_Glyph(glyph);};

      if (border)
        ENFORCE(!FT_Glyph_StrokeBorder(&glyph, ft_stroker_, 0, 1));

      ENFORCE(!FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, NULL, 1));
      FT_BitmapGlyph bit_glyph = (FT_BitmapGlyph) glyph;
      FT_Bitmap& bitmap = bit_glyph->bitmap;

      VGImage image{};
      VGfloat glyph_origin[2]{};

      if (bitmap.width > 0 && bitmap.rows > 0) {
        constexpr VGfloat blur_stddev = 0.6;
        const int padding = static_cast<int>(3*blur_stddev + 0.5);
        const int image_width = bitmap.width + padding*2;
        const int image_height = bitmap.rows + padding*2;

        image = vgCreateImage(VG_A_8, image_width, image_height,
                              VG_IMAGE_QUALITY_NONANTIALIASED);
        assert(image);
        
        if (bitmap.pitch > 0) {
          vgImageSubData(image,
                         bitmap.buffer + bitmap.pitch*(bitmap.rows-1),
                         -bitmap.pitch,
                         VG_A_8,
                         padding,
                         padding,
                         bitmap.width,
                         bitmap.rows);
          assert(!vgGetError());
        } else {
          vgImageSubData(image,
                         bitmap.buffer,
                         bitmap.pitch,
                         VG_A_8,
                         padding,
                         padding,
                         bitmap.width,
                         bitmap.rows);
          assert(!vgGetError());
        }

        auto softened_image = vgCreateImage(VG_A_8,
                                            image_width,
                                            image_height,
                                            VG_IMAGE_QUALITY_NONANTIALIASED);
        assert(image);

        // Even out hard and soft edges
        vgGaussianBlur(softened_image, image, blur_stddev, blur_stddev, VG_TILE_FILL);
        assert(!vgGetError());

        vgDestroyImage(image);
        assert(!vgGetError());

        image = softened_image;

        glyph_origin = {
            static_cast<VGfloat>(padding - bit_glyph->left),
            static_cast<VGfloat>(padding + bitmap.rows - bit_glyph->top - 1)
        };
      }

      escapement = {
          static_cast<VGfloat>((ft_face->glyph->advance.x + 32) / 64),
          0
      };

      vgSetGlyphToImage(vg_font, codepoint, image, glyph_origin, escapement);
      assert(!vgGetError());

      if (image) {
        vgDestroyImage(image);
        assert(!vgGetError());
      }
    } catch(...) {
      escapement = {0,0};
      vgSetGlyphToImage(vg_font, codepoint, VG_INVALID_HANDLE, escapement, escapement);
      assert(!vgGetError());
    }
  };

  load_glyph_internal(ft_face_, vg_font_, false);
  glyphs_[codepoint].advance = escapement[0];
  load_glyph_internal(ft_face_, vg_font_border_, true);
}

int SubtitleRenderer::get_text_width(const std::vector<InternalChar>& text) {
  int width = 0;
  for (auto c = text.begin(); c != text.end(); ++c) {
    width += c->italic ? (assert(0), 0)
                       : glyphs_.at(c->codepoint).advance;
  }
  return width;
}

std::vector<SubtitleRenderer::InternalChar> SubtitleRenderer::
get_internal_chars(const std::string& str) {
  std::vector<InternalChar> internal_chars;
  try {
    bool italic{};
    for (auto p = &str[0], e = p + str.size(); p != e;) {
      auto const cp = folly::decodeUtf8(p, e);
      internal_chars.push_back(InternalChar({cp, italic}));
    }
  } catch(...) {}
  return internal_chars;
}

void SubtitleRenderer::
prepare_glyphs(const std::vector<InternalChar>& text) {
  for (auto c = text.begin(); c != text.end(); ++c) {
    if (glyphs_.find(c->codepoint) == glyphs_.end()) {
      load_glyph(c->codepoint);
    }
  }
}

void SubtitleRenderer::
draw_text(VGFont font, VGFont italic_font,
          const std::vector<SubtitleRenderer::InternalChar>& text,
          int x, int y,
          unsigned int lightness) {
  VGPaint paint = vgCreatePaint();
  assert(paint);

  vgSetColor(paint, (lightness<<8) | (lightness<<16) | (lightness<<24) | 0xFF);
  assert(!vgGetError());

  vgSetPaint(paint, VG_FILL_PATH);
  assert(!vgGetError());

  vgDestroyPaint(paint);
  assert(!vgGetError());

  vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY);
  assert(!vgGetError());

  VGfloat pos[] = {static_cast<VGfloat>(x), static_cast<VGfloat>(y)};

  vgSetfv(VG_GLYPH_ORIGIN, 2, pos);
  assert(!vgGetError());

  for (auto c = text.begin(); c != text.end(); ++c) {
    vgDrawGlyph(c->italic ? italic_font : font,
                c->codepoint,
                VG_FILL_PATH,
                VG_FALSE);
    assert(!vgGetError());
  }
}


SubtitleRenderer::~SubtitleRenderer() {
  destroy();
}

SubtitleRenderer::
SubtitleRenderer(int layer,
                 const std::string& font_path,
                 float font_size,
                 float margin_left,
                 float margin_bottom,
                 unsigned int white_level,
                 unsigned int box_opacity):
prepared_(),
dispman_element_(),
dispman_display_(),
screen_width_(),
screen_height_(),
display_(),
context_(),
surface_(),
vg_font_(),
vg_font_border_(),
vg_font_italic_(),
vg_font_italic_border_(),
ft_library_(),
ft_face_(),
ft_stroker_(),
line_height_(),
box_offset_(),
box_h_padding_(),
margin_left_(margin_left),
margin_bottom_(margin_bottom),
white_level_(white_level),
box_opacity_(box_opacity) {
  try {
    initialize_window(layer);
    initialize_egl();
    initialize_fonts(font_path, font_size);
  } catch (...) {
    destroy();
    throw;
  }
}

void SubtitleRenderer::destroy() {
  destroy_fonts();
  destroy_egl();
  destroy_window();
}

void SubtitleRenderer::
initialize_fonts(const std::string& font_path, float font_size) {
  ENFORCE(!FT_Init_FreeType(&ft_library_));
  ENFORCE(!FT_New_Face(ft_library_, font_path.c_str(), 0, &ft_face_));
  ENFORCE(!FT_Set_Pixel_Sizes(ft_face_, 0, font_size*screen_height_));

  auto get_bbox = [this](char32_t cp) {
    auto glyph_index = FT_Get_Char_Index(ft_face_, cp);
    ENFORCE(!FT_Load_Glyph(ft_face_, glyph_index, FT_LOAD_NO_HINTING));
    FT_Glyph glyph;
    ENFORCE(!FT_Get_Glyph(ft_face_->glyph, &glyph));
    SCOPE_EXIT {FT_Done_Glyph(glyph);};
    FT_BBox bbox;
    FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &bbox);
    return bbox;
  };

  constexpr float padding_factor = 0.05f;
  int y_min = get_bbox('g').yMin;
  int y_max = get_bbox('M').yMax;
  y_max += -y_min*0.7f;
  line_height_ = y_max - y_min;
  const int v_padding = line_height_*padding_factor + 0.5f;
  line_height_ += v_padding*2;
  box_offset_ = y_min-v_padding;
  box_h_padding_ = line_height_/5.0f + 0.5f;


  constexpr float border_thickness = 0.045f;
  ENFORCE(!FT_Stroker_New(ft_library_, &ft_stroker_));
  FT_Stroker_Set(ft_stroker_,
                 line_height_*border_thickness*64.0f,
                 FT_STROKER_LINECAP_ROUND,
                 FT_STROKER_LINEJOIN_ROUND,
                 0);

  vgSeti(VG_FILTER_FORMAT_LINEAR, VG_TRUE);
  assert(!vgGetError());

  vgSeti(VG_IMAGE_QUALITY, VG_IMAGE_QUALITY_NONANTIALIASED);
  assert(!vgGetError());

  auto create_vg_font = [](VGFont& font) {
    font = vgCreateFont(128);
    ENFORCE(font);
  };

  create_vg_font(vg_font_);
  create_vg_font(vg_font_border_);
}

void SubtitleRenderer::destroy_fonts() {
  if (ft_library_) {
    auto error = FT_Done_FreeType(ft_library_);
    assert(!error);
    ft_library_ = {};
    ft_face_ = {};
    ft_stroker_ = {};
  }
} 

void SubtitleRenderer::initialize_window(int layer) {
  ENFORCE(graphics_get_display_size(0 /* LCD */, &screen_width_, &screen_height_)
          >= 0);

  VC_RECT_T dst_rect;
  dst_rect.x = 0;
  dst_rect.y = 0;
  dst_rect.width = screen_width_;
  dst_rect.height = screen_height_;
     
  VC_RECT_T src_rect;
  src_rect.x = 0;
  src_rect.y = 0;
  src_rect.width = screen_width_ << 16;
  src_rect.height = screen_height_ << 16;        

  dispman_display_ = vc_dispmanx_display_open(0 /* LCD */);
  ENFORCE(dispman_display_);

  {
    auto dispman_update = vc_dispmanx_update_start(0);
    ENFORCE(dispman_update);
    SCOPE_EXIT {
      ENFORCE(!vc_dispmanx_update_submit_sync(dispman_update));
    };

    dispman_element_ =
        vc_dispmanx_element_add(dispman_update,
                                dispman_display_,
                                layer,
                                &dst_rect,
                                0 /*src*/,
                                &src_rect,
                                DISPMANX_PROTECTION_NONE,
                                0 /*alpha*/,
                                0 /*clamp*/,
                                (DISPMANX_TRANSFORM_T) 0 /*transform*/);
    ENFORCE(dispman_element_);
  }
}

void SubtitleRenderer::destroy_window() {  
  if (dispman_element_) {
    auto dispman_update = vc_dispmanx_update_start(0);
    assert(dispman_update);

    if (dispman_update) {
      auto error = vc_dispmanx_element_remove(dispman_update, dispman_element_);
      assert(!error);

      error = vc_dispmanx_update_submit_sync(dispman_update);
      assert(!error);
    }

    dispman_element_ = {};
  }

  if (dispman_display_) {
    auto error = vc_dispmanx_display_close(dispman_display_);
    assert(!error);

    dispman_display_ = {};
  }
}

void SubtitleRenderer::initialize_egl() {
  // get an EGL display connection
  display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  ENFORCE(display_);

  // initialize the EGL display connection
  ENFORCE(eglInitialize(display_, NULL, NULL));

  // get an appropriate EGL frame buffer configuration
  static const EGLint attribute_list[] = {
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_NONE
  };
  EGLConfig config{};
  EGLint num_config{};

  ENFORCE(eglChooseConfig(display_, attribute_list, &config, 1, &num_config));
  ENFORCE(num_config);

  ENFORCE(eglBindAPI(EGL_OPENVG_API));

  static EGL_DISPMANX_WINDOW_T nativewindow;
  nativewindow.element = dispman_element_;
  nativewindow.width = screen_width_;
  nativewindow.height = screen_height_;
     
  surface_ = eglCreateWindowSurface(display_, config, &nativewindow, NULL);
  ENFORCE(surface_);

  // create an EGL rendering context
  context_ = eglCreateContext(display_, config, EGL_NO_CONTEXT, NULL);
  ENFORCE(context_);

  auto result = eglMakeCurrent(display_, surface_, surface_, context_);
  assert(result);
}

void SubtitleRenderer::destroy_egl() {
  if (display_) {
    auto result =
      eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    assert(result);

    result = eglTerminate(display_);
    assert(result);

    context_ = {};
    surface_ = {};
    display_ = {};
  }
}

void SubtitleRenderer::prepare(const std::vector<std::string>& text_lines) {
  const int n_lines = text_lines.size();

  internal_lines_.resize(n_lines);
  line_widths_.resize(n_lines);
  for (int i = 0; i < n_lines; ++i) {
    internal_lines_[i] = get_internal_chars(text_lines[i]);
    prepare_glyphs(internal_lines_[i]);
    line_widths_[i] = get_text_width(internal_lines_[i]);
  }

  const int x = (screen_width_-screen_height_) / 2 +
                static_cast<int>(margin_left_*screen_height_ + 0.5f);
  const int y = margin_bottom_*screen_height_ + 0.5f;

  line_positions_.resize(n_lines);
  for (int i = n_lines-1, line_y = y; i >= 0; --i, line_y += line_height_) {
    line_positions_[i] = {x, line_y};
  }

  prepared_ = true;
}

void SubtitleRenderer::clear() {
  vgClear(0, 0, screen_width_, screen_height_);
  assert(!vgGetError());
}

void SubtitleRenderer::draw() {
  clear();

  const auto n_lines = internal_lines_.size();

  {
    BoxRenderer box_renderer(box_opacity_);
    for (size_t i = 0; i < n_lines; ++i) {
      box_renderer.push(line_positions_[i].first - box_h_padding_,
                        line_positions_[i].second + box_offset_,
                        line_widths_[i] + box_h_padding_*2,
                        line_height_);
    }
    box_renderer.render();
  }

  for (size_t i = 0; i < n_lines; ++i) {
    draw_text(vg_font_border_, vg_font_italic_border_,
              internal_lines_[i],
              line_positions_[i].first, line_positions_[i].second,
              0);
  }

  for (size_t i = 0; i < n_lines; ++i) {
    draw_text(vg_font_, vg_font_italic_,
              internal_lines_[i],
              line_positions_[i].first, line_positions_[i].second,
              white_level_);
  }

  prepared_ = false;
}

void SubtitleRenderer::swap_buffers() {
  EGLBoolean result = eglSwapBuffers(display_, surface_);
  assert(result);
}