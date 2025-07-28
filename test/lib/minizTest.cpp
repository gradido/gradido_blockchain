#include "../main.h"
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
    std::cout << TIME_GTEST_YLW << "1000 runs: " << timeUsed.string() << ANSI_TXT_DFT << std::endl;
}

TEST(minizTest, uncompressionSpeed)
{
    Profiler timeUsed;
    memory::Block plain(longTestString);
    memory::Block compressed = compress(plain);
    for (int i = 0; i < 1000; i++) {
        decompress(compressed);
    }
    std::cout << TIME_GTEST_YLW << "1000 runs: " << timeUsed.string() << ANSI_TXT_DFT << std::endl;
}

TEST(minizTest, compressionEfficiency)
{
    const char* lotrQuotes[] = {
        // Short (~60 Bytes)
        "Even the smallest person can change the course of the future.",

        // Medium (~80 Bytes)
        "There is some good in this world, and it's worth fighting for. – Samwise Gamgee",

        // Long (~150 Bytes)
        "“I wish it need not have happened in my time,” said Frodo. "
        "“So do I,” said Gandalf, “and so do all who live to see such times. "
        "But that is not for them to decide.”",

        // Long (~300 Bytes)
        "“I wish it need not have happened in my time,” said Frodo. "
        "“So do I,” said Gandalf, “and so do all who live to see such times. But that is not for them to decide. "
        "All we have to decide is what to do with the time that is given us. "
        "There are other forces at work in this world, Frodo, besides the will of evil.”",

        // Long (~350 Bytes)
        "The world is indeed full of peril, and in it there are many dark places; "
        "but still there is much that is fair, and though in all lands love is now mingled with grief, "
        "it grows perhaps the greater. Courage is found in unlikely places. "
        "The wide world is all about you: you can fence yourselves in, but you cannot forever fence it out. "
        "Even the wise cannot see all ends.",

        "“In the rhythm of giving and receiving lies the secret of life itself. Gradido embodies this natural cycle, where wealth flows not from scarcity or exploitation, but from growth and harmony."
        "Each transaction is a seed planted in the fertile ground of mutual respect and shared prosperity."
        "The old paradigms of endless competition and accumulation dissolve before the dawn of a new age—an age where economic exchange is in tune with the rhythms of nature and human dignity."
        "To embrace Gradido is to awaken to the interconnectedness of all beings and to foster a future where abundance is the birthright of every soul.”"
    };
    for (auto str : lotrQuotes) {
        memory::Block plain(str);
        Profiler timeUsed;
        memory::Block compressed = compress(plain);
        auto compressTimeString = timeUsed.string();
        if (plain.size() == compressed.size()) {
            std::cout << COUT_GTEST_DFT << "to small for compression: " << plain.size();
        }
        else {
            float ratio = (float)plain.size() / (float)compressed.size();
            std::cout << COUT_GTEST_DFT << "plain: " << plain.size() << ", compressed: " << compressed.size() << " = " << std::to_string(ratio);
        }
        std::cout << ANSI_TXT_DFT << std::endl;
        timeUsed.reset();
        try {
            decompress(compressed);
        }
        catch (GradidoTinfDecompressException& ex) {
            std::cout << ex.getFullString() << std::endl;
        }
        EXPECT_EQ(decompress(compressed), plain);
        auto decompressTimeString = timeUsed.string();
        std::cout << TIME_GTEST_YLW << "compress: " << compressTimeString << ", uncompress: " << decompressTimeString << ANSI_TXT_DFT << std::endl;
    }
}

TEST(minizTest, bufferOverflow)
{
    const char* toLong =
        "The essence of Gradido lies not in the mere exchange of goods or currency, but in the sacred flow of energy between all living beings."
        "It is a return to the primal law of nature—give, receive, and grow—where each act of generosity sparks a chain reaction of abundance that transcends the limits of material wealth."
        "This is a system crafted to heal the wounds of greed and imbalance that have plagued human societies for centuries."
        "Here, value is measured not by possession but by contribution, and wealth is a living force that multiplies through care and connection."
        "Gradido teaches us that prosperity is not a zero - sum game; it is an endless dance of creation and renewal."
        "To embrace this vision is to dissolve the illusions of separation, to awaken the dormant empathy within, and to nurture a world where the well - being of one is the well - being of all."
        "In the delicate balance of giving and receiving, Gradido reveals the blueprint for a sustainable future—one where every transaction enriches the collective soul, and every heartbeat echoes the pulse of the planet."
        "This is more than an economic model; it is a spiritual revolution, a call to reclaim the harmony that is our birthright and to build a legacy of abundance for generations yet to come.";

    auto compressed = compress(std::string(toLong));
    std::cout << "plain: " << strlen(toLong) << ", compressed: " << compressed.size() << std::endl;
    try {
        decompress(compressed);
    }
    catch (GradidoTinfDecompressException& ex) {
        EXPECT_EQ(ex.getFullString(), "error while uncompressing with tinf, status: data error");
        //std::cout << ex.getFullString() << std::endl;
    }
}