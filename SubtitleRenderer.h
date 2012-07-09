#pragma once

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

#include <EGL/egl.h>
#include <VG/openvg.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
#include <vector>
#include <unordered_map>
#include <string>

class SubtitleRenderer {
public:
	//TODO - unsure what these do :S
  SubtitleRenderer(const SubtitleRenderer&) = delete;
  SubtitleRenderer& operator=(const SubtitleRenderer&) = delete;
  //create renderer
  SubtitleRenderer(int level,
                   const std::string& font_path,
                   float font_size,
                   float margin_left,
                   float margin_bottom,
                   unsigned int white_level,
                   unsigned int box_opacity);
  //destroy renderer
  ~SubtitleRenderer();

  //prepare renderer to draw these text lines
  void prepare(const std::vector<std::string>& text_lines);

  //unprepare renderer
  void unprepare() {
    prepared_ = false;
  }

  //show the next line(s) of subtitles
  void show_next() {
	//ensure prepared, then draw
    if (prepared_)
      draw();
    //show on screen
    swap_buffers();
  }

  //hide subtitles
  void hide() {
	//clear screen
    clear();
    //show on screen
    swap_buffers();
    //if prepared, show next subtitles
    if (prepared_)
      draw();
  }

private:
  //single character, and whether it should be italic
  struct InternalChar {
    char32_t codepoint;
    bool italic;
  };

  //TODO erm...?
  struct InternalGlyph {
    int advance;
  };

  //draw the specified text onto the screen
  static void draw_text(VGFont font, VGFont italic_font,
                        const std::vector<InternalChar>& text,
                        int x, int y,
                        unsigned int lightness);

  //destroy something? TODO
  void destroy();
  //initialise specified font
  void initialize_fonts(const std::string& font_name, float font_size);
  //destroy loaded fotns
  void destroy_fonts();
  //setup egl
  void initialize_egl();
  //destroy egl instance
  void destroy_egl();
  //initialise window to draw to
  void initialize_window(int layer);
  //destroy render window
  void destroy_window();
  //clear screen
  void clear();
  //draw to screen
  void draw();
  //switch display buffers
  void swap_buffers();
  //prepate text to draw
  void prepare_glyphs(const std::vector<InternalChar>& text);
  //load a glyph to be drawn?
  void load_glyph(char32_t codepoint);
  //calculate width of text
  int get_text_width(const std::vector<InternalChar>& text);
  //get the chars (special form) of a string
  std::vector<InternalChar> get_internal_chars(const std::string& str);

  //is renderer prepared to draw something
  bool prepared_;
  //display handles
  DISPMANX_ELEMENT_HANDLE_T dispman_element_;
  DISPMANX_DISPLAY_HANDLE_T dispman_display_;
  //screen dimensions
  uint32_t screen_width_;
  uint32_t screen_height_;
  //egl handles
  EGLDisplay display_;
  EGLContext context_;
  EGLSurface surface_;
  //font stuff
  VGFont vg_font_;
  VGFont vg_font_border_;
  VGFont vg_font_italic_;
  VGFont vg_font_italic_border_;
  //freetype stuff
  FT_Library ft_library_;
  FT_Face ft_face_;
  FT_Stroker ft_stroker_;
  //TODO glyph something?
  std::unordered_map<char32_t,InternalGlyph> glyphs_;
  //line data
  std::vector<std::vector<InternalChar>> internal_lines_;
  //line positions?
  std::vector<std::pair<int,int>> line_positions_;
  //line widths
  std::vector<int> line_widths_;
  //line height
  int line_height_;
  //box stuff
  int box_offset_;
  int box_h_padding_;
  //margins
  float margin_left_;
  float margin_bottom_;
  //white level of text
  unsigned int white_level_;
  //box opacity
  unsigned int box_opacity_;
};
