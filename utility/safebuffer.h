/*
 * prisma-realtime analyzes and visualizes monophonic musical instrument tones
 * Copyright (C) 2014  Michael Bernhard
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef SAFE_BUFFER_H
#define SAFE_BUFFER_H

template< class T >
class SafeBuffer
{
public:
	SafeBuffer(int size)
	{
		s = size + 1;
		b = new T[s];
		h = 0;
		t = 0;
	}

	~SafeBuffer()
	{
		delete[] b;
	}

	bool push_back(T v)
	{
		// Make a copy of the head index
		unsigned int i = h;

		// Increment the index
		i++;
		if(i == s)
			i = 0;

		// Check if the buffer is full
		if(i == t)
			return false;

		// Write the value in the buffer
		b[i] = v;
		// Write the new index back
		h = i;

		return true;
	}

	bool pop_front(T * v)
	{
		// Save the tail index
		unsigned int i = t;

		// Check if data is available
		if(i == h)
			return false;

		// Increment index
		i++;
		if(i == s)
			i = 0;

		// Read value from buffer
		*v = b[i];
		// Write new index back
		t = i;

		return true;
	}

private:
	unsigned int h;
	unsigned int t;
	unsigned int s;
	T * b;
};

#endif // SAFE_BUFFER_H
