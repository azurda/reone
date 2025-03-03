/*
 * Copyright (c) 2020-2021 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "files.h"
#include "options.h"
#include "soundinstance.h"

namespace reone {

namespace audio {

class SoundHandle;

class AudioPlayer : boost::noncopyable {
public:
    AudioPlayer(AudioOptions opts, AudioFiles *files);
    ~AudioPlayer();

    void init();
    void deinit();

    std::shared_ptr<SoundHandle> play(const std::string &resRef, AudioType type, bool loop = false, float gain = 1.0f, bool positional = false, glm::vec3 position = glm::vec3(0.0f));
    std::shared_ptr<SoundHandle> play(const std::shared_ptr<AudioStream> &stream, AudioType type, bool loop = false, float gain = 1.0f, bool positional = false, glm::vec3 position = glm::vec3(0.0f));

    void setListenerPosition(const glm::vec3 &position);

private:
    AudioOptions _opts;
    AudioFiles *_files;

    ALCdevice *_device { nullptr };
    ALCcontext *_context { nullptr };
    std::thread _thread;
    std::atomic_bool _run { true };
    std::vector<std::shared_ptr<SoundInstance>> _sounds;
    std::recursive_mutex _soundsMutex;
    std::atomic<glm::vec3> _listenerPosition;
    std::atomic_bool _listenerPositionDirty { false };

    void threadStart();

    void initAL();
    void deinitAL();

    void enqueue(const std::shared_ptr<SoundInstance> &sound);

    float getGain(AudioType type, float gain) const;
};

} // namespace audio

} // namespace reone
