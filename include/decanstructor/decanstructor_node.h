#ifndef DECANSTRUCTOR_NODE_H
#define DECANSTRUCTOR_NODE_H

#include <memory>
#include <cstdio>
#include <mutex>
#include <unordered_map>
#include <iomanip>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/grid.h>

#include <ros/ros.h>
#include <can_msgs/Frame.h>

namespace DeCANstructor
{
  wxDECLARE_EVENT(wxEVT_CMD_UPDATE_GRID, wxThreadEvent);

  struct CanMsgDetail
  {
    std::vector<uint8_t> bytes;
    std::vector<uint8_t> last_bytes;
    std::vector<uint64_t> last_updated_ms;
    uint64_t time_rcvd_ms = 0;
    uint64_t time_last_rcvd_ms = 0;
    unsigned int avg_rate = 0;
    int table_index;
  };

  struct CellUpdate
  {
    int row;
    int col;
    uint64_t time_diff;
  };

  class DCOptions
  {
    public:
      static uint16_t fade_out_time_ms;
      static ros::Time one_day_ago;
  };

  class DCRenderTimer :
    public wxTimer
  {
    public:
      void Notify();
  };

  class DCFrame :
    public wxFrame
  {
    public:
      DCFrame(const wxString& title,
              const wxPoint& pos,
              const wxSize& size);

      std::shared_ptr<wxGrid> main_grid;
      std::shared_ptr<wxCheckListBox> selector_box;
      std::shared_ptr<DCRenderTimer> render_timer;

    private:
      void OnExit(wxCommandEvent& event);
      void OnAbout(wxCommandEvent& event);
      void OnMainGridUpdate(wxThreadEvent& event);

      wxDECLARE_EVENT_TABLE();
  };

  class DCRosNode
  {
    public:
      DCRosNode();
      ~DCRosNode();

      void CanCallback(const can_msgs::Frame::ConstPtr& msg);

    private:
      std::unique_ptr<ros::NodeHandle> node_handle;
      std::unique_ptr<ros::NodeHandle> private_handle;
      std::unique_ptr<ros::AsyncSpinner> spinner;
      ros::Subscriber can_sub;
  };

  class DCNode :
    public wxApp
  {
    public:
      virtual bool OnInit();
      DCFrame* frame;
      std::unordered_map<uint32_t, std::shared_ptr<CanMsgDetail>> rcvd_msgs;
      std::mutex rcvd_msgs_mut;

    private:
      std::unique_ptr<DCRosNode> ros_node;
  };

	wxBEGIN_EVENT_TABLE(DCFrame, wxFrame)
		EVT_MENU(wxID_EXIT,  DCFrame::OnExit)
		EVT_MENU(wxID_ABOUT, DCFrame::OnAbout)
	wxEND_EVENT_TABLE()

  wxDEFINE_EVENT(wxEVT_CMD_UPDATE_GRID, wxThreadEvent);
}

wxIMPLEMENT_APP(DeCANstructor::DCNode);
wxDECLARE_APP(DeCANstructor::DCNode);

#endif
