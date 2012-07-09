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

class SubtitleRenderer {
public:
	//TODO - unsure what these do :S
  SubtitleRenderer(const SubtitleRenderer&) = delete;
  SubtitleRenderer& operator=(const SubtitleRenderer&) = delete;
  //create renderer
  SubtitleRenderer(int level);
  //destroy renderer
  ~SubtitleRenderer();

  //show the next line(s) of subtitles
  void show_next() {
	// draw
    draw();
    //show on screen
    swap_buffers();
  }

private:

  //destroy something? TODO
  void destroy();
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
};
