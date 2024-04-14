#include "nwpwin.h"

// TODO: prepare class ("STATIC") for a ship
class ship : public vsite::nwp::window {
	std::string class_name() override {
		return "STATIC";
	}
};

class main_window : public vsite::nwp::window
{
	ship myShip;
	static bool shipIsCreated;

protected:
	void on_left_button_down(POINT p) override { 
		// Create ship if it doesn't exist yet.
		if (!shipIsCreated) {
			myShip.create(*this, WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE, "x", 0, p.x - 10, p.y - 10, 20, 20);
			shipIsCreated = true;
		}
		// Change current location.
		else {
			HWND hw = myShip.operator HWND();
			::SetWindowPos(hw, 0, p.x - 10, p.y - 10, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
	}
	void on_key_up(int vk) override {
		// TODO: mark ship (if exists) as "not moving"
	}
	void on_key_down(int vk) override {
		// TODO: if ship exists, move it depending on key and mark as "moving"
	}
	void on_destroy() override {
		::PostQuitMessage(0);
	}
private:
};

bool main_window::shipIsCreated = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
	vsite::nwp::application app;
	main_window w;
	w.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, "NWP 4", 0, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300);
	return app.run();
}
