
#include "KeyPairs.h"
#include "sodium.h"

#include <cstring>
#include <array>

std::vector<KeyPair> g_KeyPairs;

void generateKeyPairs()
{
	unsigned char hash[crypto_hash_sha512_BYTES];

    std::array seeds = {
        "A medium of exchange for the people",
        "Love and health",
        "Three cheers",
        "We make the Earth a paradise for all beings",
        "Gratitude balance",
        "Follows its call",
        "The present is theirs; the future, for which I have really worked, is mine. - Nikola Tesla",
        "Imagination is more important than knowledge. - Albert Einstein",
        "I have not failed. I've just found 10,000 ways that won't work. - Thomas Edison",
        "The only way to do great work is to love what you do. - Steve Jobs",
        "The true sign of intelligence is not knowledge but imagination. - Albert Einstein",
        "If you want to find the secrets of the universe, think in terms of energy, frequency, and vibration. - Nikola Tesla",
        "Success is not final, failure is not fatal: It is the courage to continue that counts. - Winston Churchill",
        "To invent, you need a good imagination and a pile of junk. - Thomas Edison",
        "In the middle of every difficulty lies opportunity. - Albert Einstein",
        "Our virtues and our failings are inseparable, like force and matter. When they separate, man is no more. - Nikola Tesla",
        "The important thing is not to stop questioning. Curiosity has its own reason for existence. - Albert Einstein",
        "The scientists of today think deeply instead of clearly. One must be sane to think clearly, but one can think deeply and be quite insane. - Nikola Tesla",
        "Life is like riding a bicycle. To keep your balance, you must keep moving. - Albert Einstein",
        "Genius is one percent inspiration, ninety-nine percent perspiration. - Thomas Edison"
    };
	g_KeyPairs.reserve(seeds.size());

	for (int i = 0; i < seeds.size(); i++)
    {
		KeyPair keyPair;
		auto& seed = seeds[i];
		crypto_hash_sha512(hash, (const unsigned char*)seed, strlen(seed));
		crypto_sign_seed_keypair(*keyPair.publicKey, *keyPair.privateKey, hash);
		g_KeyPairs.push_back(keyPair);

	}

}

memory::ConstBlockPtr sign(memory::ConstBlockPtr bodyBytes, const KeyPair& keyPair)
{
    auto sign = std::make_shared<memory::Block>(crypto_sign_BYTES);
    unsigned long long actualSignLength = 0;
    crypto_sign_detached(*sign, &actualSignLength, *bodyBytes, bodyBytes->size(), *keyPair.privateKey);
    assert(actualSignLength == crypto_sign_BYTES);
    return sign;
}
