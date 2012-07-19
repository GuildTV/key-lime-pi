#include "OMXLink.h"

OMXLink::OMXLink() {
    g_abort               = false;
m_bMpeg               = false;
 m_Deinterlace         = false;
m_Pause               = false;
 m_buffer_empty        = true;
m_thread_player       = false;
m_av_clock           = NULL;
m_omx_pkt            = NULL;
m_stop                = false;
m_has_video           = false;
}

OMXLink::~OMXLink()
{
    //dtor
}

void sig_handler(int s)
{
  printf("strg-c catched\n");
  signal(SIGINT, SIG_DFL);
  //g_abort = true;
}

void OMXLink::SetSpeed(int iSpeed)
{
  if(!m_av_clock)
    return;

  if(iSpeed < OMX_PLAYSPEED_PAUSE)
    return;

  m_omx_reader.SetSpeed(iSpeed);

  if(m_av_clock->OMXPlaySpeed() != OMX_PLAYSPEED_PAUSE && iSpeed == OMX_PLAYSPEED_PAUSE)
    m_Pause = true;
  else if(m_av_clock->OMXPlaySpeed() == OMX_PLAYSPEED_PAUSE && iSpeed != OMX_PLAYSPEED_PAUSE)
    m_Pause = false;

  m_av_clock->OMXSpeed(iSpeed);
}

void OMXLink::FlushStreams(double pts)
{
  if(m_has_video)
    m_player_video.Flush();

  if(m_omx_pkt)
  {
    m_omx_reader.FreePacket(m_omx_pkt);
    m_omx_pkt = NULL;
  }

  if(pts != DVD_NOPTS_VALUE)
    m_av_clock->OMXUpdateClock(pts);
}

int OMXLink::Run() {

  signal(SIGINT, sig_handler);

  struct termios new_termios;

  tcgetattr(STDIN_FILENO, &orig_termios);

  new_termios             = orig_termios;
  new_termios.c_lflag     &= ~(ICANON | ECHO | ECHOCTL | ECHONL);
  new_termios.c_cflag     |= HUPCL;
  new_termios.c_cc[VMIN]  = 0;

  std::string last_sub = "";

  tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
  on_exit(restore_termios, &orig_termios);

  std::string            m_filename;
  CRBP                  g_RBP;
  COMXCore              g_OMX;


  m_filename = "new.mp4";

  CLog::Init("./");

  g_RBP.Initialize();
  g_OMX.Initialize();

  m_av_clock = new OMXClock();

  m_thread_player = true;

  if(!m_omx_reader.Open(m_filename.c_str(), false))
    goto do_exit;

  m_bMpeg         = m_omx_reader.IsMpegVideo();
  m_has_video     = m_omx_reader.VideoStreamCount();

  if(!m_av_clock->OMXInitialize(m_has_video, false))
    goto do_exit;

  m_omx_reader.GetHints(OMXSTREAM_VIDEO, m_hints_video);

  if(m_has_video && !m_player_video.Open(m_hints_video, m_av_clock, m_Deinterlace,  m_bMpeg,
                                         false, m_thread_player))
    goto do_exit;

  m_my_render.Open(m_av_clock, m_thread_player, &m_player_video, m_filename);

  m_av_clock->SetSpeed(DVD_PLAYSPEED_NORMAL);
  m_av_clock->OMXStateExecute();
  m_av_clock->OMXStart();

  while(!m_stop)
  {
    if(g_abort)
      goto do_exit;

    if(m_Pause)
    {
      OMXClock::OMXSleep(2);
      continue;
    }

    if(!m_omx_pkt)
      m_omx_pkt = m_omx_reader.Read();

    if(m_has_video && m_omx_pkt && m_omx_reader.IsActive(OMXSTREAM_VIDEO, m_omx_pkt->stream_index))
    {
      if(m_player_video.AddPacket(m_omx_pkt))
        m_omx_pkt = NULL;
      else
        OMXClock::OMXSleep(10);
    }
    else
    {
      if(m_omx_pkt)
      {
        m_omx_reader.FreePacket(m_omx_pkt);
        m_omx_pkt = NULL;
      }
    }

    if(m_omx_reader.IsEof())
        break;

  }

do_exit:
  printf("\n");

  if(!m_stop)
  {
	if(m_has_video)
      m_player_video.WaitCompletion();
  }

  m_av_clock->OMXStop();
  m_av_clock->OMXStateIdle();

  m_player_video.Close();

  m_my_render.Close();

  if(m_omx_pkt)
  {
    m_omx_reader.FreePacket(m_omx_pkt);
    m_omx_pkt = NULL;
  }

  m_omx_reader.Close();

  g_OMX.Deinitialize();
  g_RBP.Deinitialize();

  return 1;
}

int main (int argc, char *argv[]){
    OMXLink omx;
    omx.Run();
    return 1;
}
