/*****************************************************************************
**                                    / _|
**   __ _ _   _ _ __ ___  _ __ __ _  | |_ ___  ___ ___
**  / _` | | | | '__/ _ \| '__/ _` | |  _/ _ \/ __/ __|
** | (_| | |_| | | | (_) | | | (_| | | || (_) \__ \__ \
**  \__,_|\__,_|_|  \___/|_|  \__,_| |_| \___/|___/___/
**
** Copyright (C) 2018 Aurora Free Open Source Software.
**
** This file is part of the Aurora Free Open Source Software. This
** organization promote free and open source software that you can
** redistribute and/or modify under the terms of the GNU Lesser General
** Public License Version 3 as published by the Free Software Foundation or
** (at your option) any later version approved by the Aurora Free Open Source
** Software Organization. The license is available in the package root path
** as 'LICENSE' file. Please review the following information to ensure the
** GNU Lesser General Public License version 3 requirements will be met:
** https://www.gnu.org/licenses/lgpl.html .
**
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3 or later as published by the Free Software
** Foundation. Please review the following information to ensure the GNU
** General Public License requirements will be met:
** http://www.gnu.org/licenses/gpl-3.0.html.
**
** NOTE: All products, services or anything associated to trademarks and
** service marks used or referenced on this file are the property of their
** respective companies/owners or its subsidiaries. Other names and brands
** may be claimed as the property of others.
**
** For more info about intellectual property visit: aurorafoss.org or
** directly send an email to: contact (at) aurorafoss.org .
*****************************************************************************/

/**
 * @file AuroraFW/Audio/AudioInput.H
 * AudioInput header. This contains an AudioIStream struct
 * used to handle audio input.
 * @since snapshot20180330
 */

#ifndef AURORAFW_AUDIO_AUDIO_INPUT_H
#define AURORAFW_AUDIO_AUDIO_INPUT_H

// AuroraFW
#include <AuroraFW/Global.h>
#include <AuroraFW/Audio/AudioUtils.h>
#include <AuroraFW/Audio/AudioBackend.h>

namespace AuroraFW {
	namespace AudioManager {

		/**
		 * A struct representing an audio input stream. A struct that enables the user to
		 * record audio input and store it on disk.
		 * @since snapshot20180330
		 */
		struct AFW_API AudioIStream {
			friend int audioInputCallback(const void* , void* , size_t ,
			const PaStreamCallbackTimeInfo* , PaStreamCallbackFlags, void* );

			/**
			 * Constructs an audio input stream.
			 * @param path The path to where to save the audio stream.
			 * @param info The AudioInfo object specifying all info about the stream.
			 * @param int The number of frames (without considering the number of channels) to store.
			 * @see ~AudioIStream()
			 * @since snapshot20180330
			 */
			AudioIStream(const char* , AudioInfo* , int );

			/**
			 * Destructs an audio input stream.
			 * @see AudioIStream()
			 * @since snapshot20180330
			 */
			~AudioIStream();

			/**
			 * Starts recording audio input.
			 * @throws PAErrorException In case there was a PortAudio error.
			 * @see pause()
			 * @see stop()
			 * @since snapshot20180330
			 */
			void record();

			/**
			 * Pauses the current recording.
			 * @throws PAErrorException In case there was a PortAudio error.
			 * @see record()
			 * @see stop()
			 * @since snapshot20180330
			 */
			void pause();

			/**
			 * Stops the current recording. (resets the position back to 0)
			 * @throws PAErrorException In case there was a PortAudio error.
			 * @see record()
			 * @see pause()
			 * @since snapshot20180330
			 */
			void stop();

			/**
			 * Returns if the stream is recording audio input.
			 * @return <em>true</em> if the stream is recording audio. <em>false</em> otherwise.
			 * @see isPaused()
			 * @see isStopped()
			 * @since snapshot20180330
			 */
			bool isRecording();

			/**
			 * Returns if the stream is currently paused.
			 * @return <em>true</em> if the stream was been paused. <em>false</em> otherwise.
			 * @note If the stream is stopped, it doesn't count as being paused.
			 * @see isRecording()
			 * @see isStopped()
			 * @since snapshot20180330
			 */
			bool isPaused();

			/**
			 * Returns if the stream is completely stopped and not just paused.
			 * @return <em>true</em> if the stream is stopped. <em>false</em> otherwise.
			 * @see isRecording()
			 * @see isPaused()
			 * @since snapshot20180330
			 */
			bool isStopped();

			/**
			 * Returns is the buffer is completely filled with audio data.
			 * @return <em>true</em> if the buffer is full. <em>false</em> otherwise.
			 * @since snapshot20180330
			 */
			bool isBufferFull();

			/**
			 * Clears the entire buffer so it can start fresh for new recordings.
			 * @warning Calling this without saving the buffer first will discard any data currently stored in the buffer.
			 * @see clearBuffer(unsigned int , unsigned int )
			 * @since snapshot20180330
			 */
			void clearBuffer();

			/**
			 * Clears only a portion of the buffer specified by the start and finish parameters.
			 * @param start The start frame.
			 * @param finish The end frame.
			 * @note The frame values above must not consider the number of channels the stream has.
			 * @see clearBuffer()
			 * @since snapshot20180330
			 */
			void clearBuffer(unsigned int start, unsigned int finish);

			/**
			 * Saves the current audio stream (wheter it's empty, full or partially filled)
			 * to the disk on the path specified in the constructor.
			 * @return <em>true</em> if the saving was successfull. <em>false</em> otherwise.
			 * @since snapshot20180330
			 */
			bool save();

			/**
			 * The path to save the audio stream to.
			 * @since snapshot20180330
			 */
			const char* path;

			/**
			 * The AudioInfo object with all the information needed.
			 * @since snapshot20180330
			 */
			AudioInfo* info;

			/**
			 * The audio stream buffer, that is equal to the product of frames and the number of channels.
			 * @see bufferSize
			 * @since snapshot20180330
			 */
			float* buffer;	// TODO - Check if this needs to be private

			/**
			 * The buffer's size, that is equal to the number of frames requested,
			 * without considering the number of channels.
			 * @see buffer
			 * @since snapshot20180330
			 */
			const int bufferSize;

		private:
			PaStream* _paStream;
			unsigned int _streamPosFrame = 0;
		};
	}
}

#endif // AURORAFW_AUDIO_AUDIO_INPUT_H
