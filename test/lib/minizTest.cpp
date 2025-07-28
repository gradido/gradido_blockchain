#include "gtest/gtest.h"
#include "gradido_blockchain/lib/minizLib.h"
#include "gradido_blockchain/lib/Profiler.h"
#include "gradido_blockchain/const.h"


// long enough for compression
const char* longTestString = 
"\"I wish it need not have happened in my time,\" said Frodo."
"\"So do I,\" said Gandalf, \"and so do all who live to see such times."
"But that is not for them to decide."
"All we have to decide is what to do with the time that is given us."
"There are other forces at work in this world, Frodo, besides the will of evil."
"That there is some good in this world, and it’s worth fighting for.\""
;
// to short for reducing one compression
const char* shortTestString = "All we have to decide is what to do with the time that is given us.";

TEST(minizTest, compressAndDecompress)
{
    memory::Block plain(longTestString);
    memory::Block compressed = compress(plain);
    memory::Block decompressed = decompress(compressed);
    EXPECT_EQ(plain, decompressed);
}

TEST(minizTest, compressionSpeed)
{
    Profiler timeUsed;
    memory::Block plain(longTestString);
    for (int i = 0; i < 1000; i++) {
        compress(plain);
    }
    std::cout << "time for 1000 runs: " << timeUsed.string() << std::endl;
}

TEST(minizTest, uncompressionSpeed)
{
    Profiler timeUsed;
    memory::Block plain(longTestString);
    memory::Block compressed = compress(plain);
    for (int i = 0; i < 1000; i++) {
        decompress(compressed);
    }
    std::cout << "time for 1000 runs: " << timeUsed.string() << std::endl;
}

TEST(minizTest, compressionEfficiency)
{
    const char* lotrQuotes[] = {
        // Short (~60 Bytes)
        "Even the smallest person can change the course of the future.",

        // Medium (~120 Bytes)
        "There is some good in this world, and it's worth fighting for. – Samwise Gamgee",

        // Long (~250 Bytes)
        "“I wish it need not have happened in my time,” said Frodo. "
        "“So do I,” said Gandalf, “and so do all who live to see such times. "
        "But that is not for them to decide.”",

        // Very Long (~500 Bytes)
        "“I wish it need not have happened in my time,” said Frodo. "
        "“So do I,” said Gandalf, “and so do all who live to see such times. But that is not for them to decide. "
        "All we have to decide is what to do with the time that is given us. "
        "There are other forces at work in this world, Frodo, besides the will of evil.”",

        // Epic (~800 Bytes)
        "The world is indeed full of peril, and in it there are many dark places; "
        "but still there is much that is fair, and though in all lands love is now mingled with grief, "
        "it grows perhaps the greater. Courage is found in unlikely places. "
        "The wide world is all about you: you can fence yourselves in, but you cannot forever fence it out. "
        "Even the wise cannot see all ends."
    };
    for (auto str : lotrQuotes) {
        memory::Block plain(str);
        Profiler timeUsed;
        memory::Block compressed = compress(plain);
        auto compressTimeString = timeUsed.string();
        if (plain.size() == compressed.size()) {
            std::cout << "to small for compression: " << plain.size();
        }
        else {
            float ratio = (float)plain.size() / (float)compressed.size();
            std::cout << "plain: " << plain.size() << ", compressed: " << compressed.size() << " = " << std::to_string(ratio);
        }
        std::cout << std::endl;
        timeUsed.reset();
        EXPECT_EQ(decompress(compressed), plain);
        auto decompressTimeString = timeUsed.string();
        std::cout << "time for compress: " << compressTimeString << ", for uncompress: " << decompressTimeString << std::endl;
    }
}