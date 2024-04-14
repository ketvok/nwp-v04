#include "nwpwin.h"

// TODO: prepare class ("STATIC") for a ship
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
	POINT& getCoordinates() {
		return p;
	}
};

class main_window : public vsite::nwp::window
{
	ship myShip;
	static bool shipIsCreated;
	bool ctrlPressed = false;
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
				HWND hw = myShip.operator HWND();
				::SetWindowPos(hw, 0, myShip.getCoordinates().x + x, myShip.getCoordinates().y + y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				POINT np{ myShip.getCoordinates().x + x, myShip.getCoordinates().y + y };
				myShip.setCoordinates(np);
			}
		}
	}

	void on_left_button_down(POINT p) override { 
		// Create ship if it doesn't exist yet.
		RECT windowSize{};  // Struktura left, right, top, bottom.
		::GetClientRect(*this, &windowSize);  // Dohvati left, right, top i bottom koordinate klijentskog dijela prozora; gornji lijevi kut je (0, 0).

		if (!shipIsCreated) {  // Ako ship ne postoji....
			if (p.x - 10 >= windowSize.left &&
				p.y - 10 >= windowSize.top &&
				p.x + 10 <= windowSize.right &&
				p.y + 10 <= windowSize.bottom) {
				myShip.create(*this, WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE, "x", 0, p.x - 10, p.y - 10, 20, 20);  // Stvori ga na željenoj poziciji.
				shipIsCreated = true;  // Postavi flag da brod postoji.
			}
		}
		// Change current location.
		else {  // Ako ship veæ postoji...
			HWND hw = myShip.operator HWND();
			if (p.x - 10 >= windowSize.left &&
				p.y - 10 >= windowSize.top &&
				p.x + 10 <= windowSize.right &&
				p.y + 10 <= windowSize.bottom) {
				::SetWindowPos(hw, 0, p.x - 10, p.y - 10, 0, 0, SWP_NOSIZE | SWP_NOZORDER);  // Postavi ga na novu poziciju.
			}
		}
		POINT nc{ p.x - 10, p.y - 10 };
		myShip.setCoordinates(nc);
	}
	void on_key_up(int vk) override {
		if (vk == VK_CONTROL) {
			ctrlPressed = false;
		}
		// TODO: mark ship (if exists) as "not moving"
		else if (vk >= VK_LEFT && vk <= VK_DOWN) {  // Ako je podignuta neka od strelica...
			isMoving = false;  // Brod se više ne mièe.
		}
	}
	void on_key_down(int vk) override {
		if (vk == VK_CONTROL) {
			ctrlPressed = true;
		}
		// TODO: if ship exists, move it depending on key and mark as "moving"
		if (shipIsCreated) {
			if (vk >= VK_LEFT && vk <= VK_DOWN) {
				isMoving = true;
				int moveDistance = ctrlPressed ? 4 : 2;
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
