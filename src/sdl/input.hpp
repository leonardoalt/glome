#pragma once

#include <functional>
#include <unordered_map>
#include <SDL.h>
#include <iostream>

#include "ship_controller.hpp"
#include "input_callback.hpp"
#include "events.hpp"
#include "game.hpp"

namespace Input
{
	/*
	* Generic Callback function for input
	* TODO: Make this work for variable number of arguments:
	* http://en.wikipedia.org/wiki/Variadic_templates
	*/
	extern std::unordered_map<int, ShipController*> ship_controllers;

	ShipController* create_ship_controller(int);

	void read_controllers_settings();

	void initialize();

	bool handle();

	inline float normalize_button(int e)
	{
		switch(e)
		{
		case SHOOT:
		case MOVE_BACKWARD:
		case MOVE_RIGHT:
		case MOVE_UP:
		case MOVE_SPINL:
			return 1.0;
		case MOVE_DOWN:
		case MOVE_LEFT:
		case MOVE_SPINR:
		case MOVE_FORWARD:
			return -1.0;
		case PAUSE:
			return 1.0;
		default:
			return 0.0;
		}
	}

	inline void rotate(int controller_id, float rx, float ry)
	{
		ship_controllers[controller_id]->rotate_h(rx);
		ship_controllers[controller_id]->rotate_v(ry);
	}

	inline void move_forward(int controller_id, float a)
	{
		ship_controllers[controller_id]->move(a);
	}

	inline void move_backward(int controller_id, float a)
	{
		ship_controllers[controller_id]->move(a);
	}

	inline void move_up(int controller_id, float a)
	{
		ship_controllers[controller_id]->move_v(a);
	}

	inline void move_down(int controller_id, float a)
	{
		ship_controllers[controller_id]->move_v(a);
	}

	inline void move_left(int controller_id, float a)
	{
		ship_controllers[controller_id]->move_h(a);
	}

	inline void move_right(int controller_id, float a)
	{
		ship_controllers[controller_id]->move_h(a);
	}

	inline void move_spinl(int controller_id, float a)
	{
		ship_controllers[controller_id]->move_s(a);
	}

	inline void move_spinr(int controller_id, float a)
	{
		ship_controllers[controller_id]->move_s(a);
	}

	inline void shoot(int controller_id, float a)
	{
		ship_controllers[controller_id]->shoot((bool)a);
	}

	inline void game_pause(int b, float a)
	{
		static bool paused = false;
		if(a > 0.5f) {
			Game::switch_state(paused ? Game::WORLD : Game::MENU);
			paused = !paused;

			SDL_SetRelativeMouseMode(SDL_bool(!paused));
		}
	}
}

