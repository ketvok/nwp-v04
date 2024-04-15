#include "nwpwin.h"

// Prepare class ("STATIC") for a ship.
class ship : public vsite::nwp::window {
	std::string class_name() override {
		return "STATIC";
	}
	POINT p;

public:
	void setCoordinates(POINT& pr) {
		p.x = pr.x;
		p.y = pr.y;
	}
	const POINT& getCoordinates() {
		return p;
	}
};

class main_window : public vsite::nwp::window
{
	ship myShip;
	static bool shipIsCreated;
	bool isMoving = false;

protected:
	void moveShip(int x, int y) {
		RECT windowSize{};
		::GetClientRect(*this, &windowSize);

		if (shipIsCreated) {
			if (myShip.getCoordinates().x + x >= windowSize.left &&
				myShip.getCoordinates().x + x + 20 <= windowSize.right &&
				myShip.getCoordinates().y + y >= windowSize.top &&
				myShip.getCoordinates().y + y + 20 <= windowSize.bottom) {
				::SetWindowLongPtr(myShip, GWL_STYLE, WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | WS_BORDER);  // Conversion operator converts ship to HWND.
				::SetWindowPos(myShip, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE);
				::SetWindowPos(myShip, 0, myShip.getCoordinates().x + x, myShip.getCoordinates().y + y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
				POINT np{ myShip.getCoordinates().x + x, myShip.getCoordinates().y + y };
				myShip.setCoordinates(np);
			}
		}
	}

	void on_left_button_down(POINT p) override { 
		// Create ship if it doesn't exist yet.
		RECT windowSize{};  // Structure left, right, top, bottom.
		::GetClientRect(*this, &windowSize);  // Fetch left, right, top and bottom coordinates of client part of window; upper left corner is (0, 0).

		if (!shipIsCreated) {  // If ship does not exist....
			if (p.x - 10 >= windowSize.left &&
				p.y - 10 >= windowSize.top &&
				p.x + 10 <= windowSize.right &&
				p.y + 10 <= windowSize.bottom) {
				myShip.create(*this, WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE, "x", 0, p.x - 10, p.y - 10, 20, 20);  // Create it at the desired position.
				shipIsCreated = true;  // Set flag that ship exists.
			}
		}
		// Change current location.
		else {  // If ship already exists...
			if (p.x - 10 >= windowSize.left &&
				p.y - 10 >= windowSize.top &&
				p.x + 10 <= windowSize.right &&
				p.y + 10 <= windowSize.bottom) {
				::SetWindowPos(myShip, 0, p.x - 10, p.y - 10, 0, 0, SWP_NOSIZE | SWP_NOZORDER);  // Place it at the new position. Conversion operator converts ship to HWND.
			}
		}
		POINT nc{ p.x - 10, p.y - 10 };  // Corrected coordinates.
		myShip.setCoordinates(nc);
	}
	void on_key_up(int vk) override {
		// Mark ship (if exists) as "not moving".
		if (vk >= VK_LEFT && vk <= VK_DOWN) {  // If arrow is lifted...
			isMoving = false;  // Ship is no longer moving.
			// Remove border of the ship that is no longer moving.
			::SetWindowLongPtr(myShip, GWL_STYLE, WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE);  // Conversion operator converts ship to HWND.
			::SetWindowPos(myShip, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE);
		}
	}
	void on_key_down(int vk) override {
		// If ship exists, move it depending on key and mark as "moving".
		if (shipIsCreated) {
			if (vk >= VK_LEFT && vk <= VK_DOWN) {
				isMoving = true;
				int moveDistance = ::GetAsyncKeyState(VK_CONTROL) ? 4 : 2;
				
				switch (vk) {
				case VK_LEFT:
					moveShip(-moveDistance, 0);
					break;
				case VK_RIGHT:
					moveShip(moveDistance, 0);
					break;
				case VK_UP:
					moveShip(0, -moveDistance);
					break;
				case VK_DOWN:
					moveShip(0, moveDistance);
					break;
				}
			}
		}
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
