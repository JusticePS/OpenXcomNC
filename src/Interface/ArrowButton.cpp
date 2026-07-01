/*
 * Copyright 2010-2016 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ArrowButton.h"
#include "TextList.h"
#include "../Engine/Timer.h"
#include "../Engine/Action.h"

namespace OpenXcom
{

/**
 * Sets up an arrow button with the specified size and position.
 * @param shape Shape of the arrow.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
ArrowButton::ArrowButton(ArrowShape shape, int width, int height, int x, int y, int remoteNumber) : ImageButton(width, height, x, y), _shape(shape), _list(0)
{
	_timer = new Timer(50);
	_timer->onTimer((SurfaceHandler)&ArrowButton::scroll);
	_remoteNumber = remoteNumber;
}

/**
 * Deletes timers.
 */
ArrowButton::~ArrowButton()
{
	delete _timer;
}

bool ArrowButton::isButtonHandled(Uint8 button)
{
	if (_list != 0)
	{
		return (button == SDL_BUTTON_LEFT || button == SDL_BUTTON_RIGHT);
	}
	else
	{
		return ImageButton::isButtonHandled(button);
	}
}

/**
 * Changes the color for the image button.
 * @param color Color value.
 */
void ArrowButton::setColor(Uint8 color)
{
	ImageButton::setColor(color);
	_redraw = true;
}

/**
 * Changes the shape for the arrow button.
 * @param shape Shape of the arrow.
 */
void ArrowButton::setShape(ArrowShape shape)
{
	_shape = shape;
	_redraw = true;
}

/**
 * Changes the list associated with the arrow button.
 * This makes the button scroll that list.
 * @param list Pointer to text list.
 */
void ArrowButton::setTextList(TextList *list)
{
	_list = list;
}

/**
 * Draws the button with the specified arrow shape.
 */
void ArrowButton::draw()
{
	ImageButton::draw();
	lock();

	// Draw button
	SDL_Rect square;
	int color = _color + 2;

	square.x = 0;
	square.y = 0;
	square.w = getWidth() - 1;
	square.h = getHeight() - 1;

	drawRect(&square, color);

	square.x++;
	square.y++;
	color = _color + 5;

	drawRect(&square, color);

	square.w--;
	square.h--;
	color = _color + 4;

	drawRect(&square, color);

	setPixel(0, 0, _color + 1);
	setPixel(0, getHeight() - 1, _color + 4);
	setPixel(getWidth() - 1, 0, _color + 4);

	color = _color + 1;

	switch (_shape)
	{
	case OpenXcom::ARROW_BIG_UP:
		// Draw arrow square
		square.x = 5;
		square.y = 8;
		square.w = 3;
		square.h = 3;

		drawRect(&square, color);

		// Draw arrow triangle
		square.x = 2;
		square.y = 7;
		square.w = 9;
		square.h = 1;

		for (; square.w > 1; square.w -= 2)
		{
			drawRect(&square, color);
			square.x++;
			square.y--;
		}
		drawRect(&square, color);
		break;
	case OpenXcom::ARROW_BIG_DOWN:
		// Draw arrow square
		square.x = 5;
		square.y = 3;
		square.w = 3;
		square.h = 3;

		drawRect(&square, color);

		// Draw arrow triangle
		square.x = 2;
		square.y = 6;
		square.w = 9;
		square.h = 1;

		for (; square.w > 1; square.w -= 2)
		{
			drawRect(&square, color);
			square.x++;
			square.y++;
		}
		drawRect(&square, color);
		break;
	case OpenXcom::ARROW_SMALL_UP:
		// Draw arrow triangle 1
		square.x = 1;
		square.y = 5;
		square.w = 9;
		square.h = 1;

		for (; square.w > 1; square.w -= 2)
		{
			drawRect(&square, color + 2);
			square.x++;
			square.y--;
		}
		drawRect(&square, color + 2);

		// Draw arrow triangle 2
		square.x = 2;
		square.y = 5;
		square.w = 7;
		square.h = 1;

		for (; square.w > 1; square.w -= 2)
		{
			drawRect(&square, color);
			square.x++;
			square.y--;
		}
		drawRect(&square, color);
		break;
	case OpenXcom::ARROW_SMALL_DOWN:
		// Draw arrow triangle 1
		square.x = 1;
		square.y = 2;
		square.w = 9;
		square.h = 1;

		for (; square.w > 1; square.w -= 2)
		{
			drawRect(&square, color + 2);
			square.x++;
			square.y++;
		}
		drawRect(&square, color + 2);

		// Draw arrow triangle 2
		square.x = 2;
		square.y = 2;
		square.w = 7;
		square.h = 1;

		for (; square.w > 1; square.w -= 2)
		{
			drawRect(&square, color);
			square.x++;
			square.y++;
		}
		drawRect(&square, color);
		break;
	case OpenXcom::ARROW_SMALL_LEFT:
		// Draw arrow triangle 1
		square.x = 2;
		square.y = 4;
		square.w = 2;
		square.h = 1;

		for (; square.h < 5; square.h += 2)
		{
			drawRect(&square, color + 2);
			square.x += 2;
			square.y--;
		}
		square.w = 1;
		drawRect(&square, color + 2);

		// Draw arrow triangle 2
		square.x = 3;
		square.y = 4;
		square.w = 2;
		square.h = 1;

		for (; square.h < 5; square.h += 2)
		{
			drawRect(&square, color);
			square.x += 2;
			square.y--;
		}
		square.w = 1;
		drawRect(&square, color);
		break;
	case OpenXcom::ARROW_SMALL_RIGHT:
		// Draw arrow triangle 1
		square.x = 7;
		square.y = 4;
		square.w = 2;
		square.h = 1;

		for (; square.h < 5; square.h += 2)
		{
			drawRect(&square, color + 2);
			square.x -= 2;
			square.y--;
		}
		square.x++;
		square.w = 1;
		drawRect(&square, color + 2);

		// Draw arrow triangle 2
		square.x = 6;
		square.y = 4;
		square.w = 2;
		square.h = 1;

		for (; square.h < 5; square.h += 2)
		{
			drawRect(&square, color);
			square.x -= 2;
			square.y--;
		}
		square.x++;
		square.w = 1;
		drawRect(&square, color);
		break;
	default:
		break;
	}

	unlock();
}

/**
 * Keeps the scrolling timers running.
 */
void ArrowButton::think()
{
	_timer->think(0, this);
}

/**
 * Scrolls the list.
 */
void ArrowButton::scroll()
{
	if (_shape == ARROW_BIG_UP)
	{
		_list->scrollUp(false);
	}
	else if (_shape == ARROW_BIG_DOWN)
	{
		_list->scrollDown(false);
	}
}

/**
 * Starts scrolling the associated list.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void ArrowButton::mousePress(Action *action, State *state)
{
	ImageButton::mousePress(action, state);
	if (_list != 0)
	{
		if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
		{
			_timer->start();
		}
		else if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP) _list->scrollUp(false, true);
		else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN) _list->scrollDown(false, true);
	}
}

/*
 * Stops scrolling the associated list.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void ArrowButton::mouseRelease(Action *action, State *state)
{
	ImageButton::mouseRelease(action, state);
	if (_list != 0 && action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timer->stop();
	}
}

/*
 * Scrolls the associated list to top or bottom.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void ArrowButton::mouseClick(Action *action, State *state)
{
	ImageButton::mouseClick(action, state);
	if (_list != 0 && SDL_BUTTON_RIGHT == action->getDetails()->button.button) {
		if (_shape == ARROW_BIG_UP) _list->scrollUp(true);
		else if (_shape == ARROW_BIG_DOWN) _list->scrollDown(true);
	}
}

/// Sends a mousepress if this matches up/down/left/right
bool ArrowButton::pressIfLabelMatches(int access_level, const std::string& textLabel, State* state, bool exactMatch, float xscale, float yscale, float topband, float leftband)
{
	if (access_level < _access_level_required)
		return false;
	std::string arrow_text = std::string("");
	switch (_shape)
	{
	case ArrowShape::ARROW_BIG_DOWN:
	case ArrowShape::ARROW_SMALL_DOWN:
		arrow_text = "down";
		break;
	case ArrowShape::ARROW_BIG_UP:
	case ArrowShape::ARROW_SMALL_UP:
		arrow_text = "up";
		break;
	case ArrowShape::ARROW_SMALL_LEFT:
		arrow_text = "left";
		break;
	case ArrowShape::ARROW_SMALL_RIGHT:
		arrow_text = "right";
		break;

	}
	int before_plus_or_minus_chars = textLabel.size();
	int remoteNumber = 0;
	size_t space_index = textLabel.find_first_of(' ');
	auto back_iterator = textLabel.end();
	if (space_index > 0 && space_index < textLabel.size())
	{
		std::string endNumSubstring = textLabel.substr(space_index + 1);
		remoteNumber = std::stoi(endNumSubstring);
		if (remoteNumber != _remoteNumber)
			return false;
		before_plus_or_minus_chars = space_index;
		for (int i = space_index; i < textLabel.size(); ++i)
		{
			back_iterator--;
		}
	}
	else
	{
		space_index = -1;
	}

	size_t plus_index = textLabel.find('+');
	size_t minus_index = textLabel.find('-');
	std::string num_substr;
	int num_presses = 1;
	
	
	if ((plus_index >= 1 && plus_index != -1) || (minus_index >= 1 && minus_index != -1))
	{
		if (plus_index >= 1 && plus_index != -1)
		{
			before_plus_or_minus_chars = plus_index;
			if (space_index > 0)
			{
				num_substr = textLabel.substr(plus_index + 1, space_index - (plus_index + 1));
			}
			else
			{
				num_substr = textLabel.substr(plus_index + 1);
			}
			back_iterator = textLabel.end();
			for (int i = plus_index; i < textLabel.size(); ++i)
			{
				back_iterator--;
			}
		}
		else
		{
			before_plus_or_minus_chars = minus_index;
			if (space_index > 0)
			{
				num_substr = textLabel.substr(minus_index + 1, space_index - (minus_index + 1));
			}
			else
			{
				num_substr = textLabel.substr(minus_index + 1);
			}
			back_iterator = textLabel.end();
			for (int i = minus_index; i < textLabel.size(); ++i)
			{
				back_iterator--;
			}
		}
		num_presses = std::stoi(num_substr);
	}
	else
	{
		plus_index = minus_index = -1;
	}

	if (num_presses > 500)
		num_presses = 500;
	if (num_presses < 1)
		num_presses = 1;

	if (before_plus_or_minus_chars <= arrow_text.size() && std::equal(textLabel.begin(), back_iterator, arrow_text.begin(), [](auto a, auto b)
															{ return std::tolower(a) == std::tolower(b); }))
	{
		for (int i = 0; i < num_presses; ++i)
		{
			{
				SDL_Event simEv;
				simEv.type = SDL_MOUSEBUTTONDOWN;
				simEv.button.button = SDL_BUTTON_LEFT;
				Action a = Action(&simEv, 0.0, 0.0, 0, 0);
				mousePress(&a, state);
			}

			{
				SDL_Event simEv;
				simEv.type = SDL_MOUSEBUTTONUP;
				simEv.button.button = SDL_BUTTON_LEFT;
				Action a = Action(&simEv, 0.0, 0.0, 0, 0);
				mouseRelease(&a, state);
			}

			{
				SDL_Event simEv;
				simEv.type = SDL_MOUSEBUTTONUP;
				simEv.button.button = SDL_BUTTON_LEFT;
				Action a = Action(&simEv, 0.0, 0.0, 0, 0);
				mouseClick(&a, state);
			}
		}
		return true;
	}
	return false;
}

}
