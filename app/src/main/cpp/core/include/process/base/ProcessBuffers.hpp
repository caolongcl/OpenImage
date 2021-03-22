//
// Created by caolong on 2021/3/8.
//

#pragma once

#include <softarch/std.hpp>
#include <softarch/IComFunc.hpp>

namespace clt {

    class Buffer;

    class ProcessBuffers final : public IComFunc<>,
                                 public IComUpdate<const std::size_t, const std::size_t> {
    public:
        using BufferQueue = std::queue<std::shared_ptr<Buffer>>;
    public:
        ProcessBuffers();

        ~ProcessBuffers() = default;

        bool Init() override;

        void Update(const std::size_t width, const std::size_t height) override;

        void DeInit() override;

        std::shared_ptr<Buffer> PopWriteBuffer();

        std::shared_ptr<Buffer> PopReadBuffer();

        void PushWriteBuffer(const std::shared_ptr<Buffer> &);

        void PushReadBuffer(const std::shared_ptr<Buffer> &);

    private:
        void resetBufferQueue();

    private:
        static const int QUEUE_SIZE = 6;

        BufferQueue m_writer;
        BufferQueue m_reader;

        std::mutex m_mutex;

        std::vector<std::shared_ptr<Buffer>> m_buffers;
    };

};
