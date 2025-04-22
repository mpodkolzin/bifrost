#include "io/writer_pool.h"
#include "io/buffered_writer.h"
#include "io/file_handle.h"



WriterPool::WriterPool() {

}


BufferedWriter* WriterPool::getWriterForShard(int shard_id, std::shared_ptr<FileHandle> file) {
    std::lock_guard lock(mutex_);

    auto it = writers_.find(shard_id);
    if (it != writers_.end()) {
        return it->second.get();
    }

    auto writer = std::make_unique<BufferedWriter>(file, 26);
    BufferedWriter* writerPtr = writer.get();
    writers_[shard_id] = std::move(writer);
    return writerPtr;

}