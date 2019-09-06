#include "DHChachaFilter.hpp"

#include "cryptlib.h"
#include "chacha.h"
#include "integer.h"
#include "dh.h"
#include "eccrypto.h"
#include "oids.h"
#include "osrng.h"
#include "secblock.h"
#include "sha.h"
#include "blake2.h"


using namespace CryptoPP::ASN1;

namespace TiltedPhoques
{
	struct DHChachaFilterPimpl
	{
		DHChachaFilterPimpl()
			: m_dh(secp256r1())
		{}

		CryptoPP::ECDH<CryptoPP::ECP>::Domain m_dh;
		CryptoPP::XChaCha20::Encryption m_cipher;
		CryptoPP::SecByteBlock m_pubKey;
		CryptoPP::SecByteBlock m_priKey;
	};

	DHChachaFilter::DHChachaFilter()
		: m_pPimpl{ GetAllocator()->New<DHChachaFilterPimpl>() }
		, m_iv{ 0 }
	{
		m_pPimpl->m_priKey.resize(m_pPimpl->m_dh.PrivateKeyLength());
		m_pPimpl->m_pubKey.resize(m_pPimpl->m_dh.PublicKeyLength());

		GenerateKeys();
	}

	DHChachaFilter::~DHChachaFilter()
	{
		GetAllocator()->Delete(m_pPimpl);
	}

	bool DHChachaFilter::PreConnect(Buffer::Writer* apBuffer)
	{
		return apBuffer->WriteBytes(m_pPimpl->m_pubKey.BytePtr(), m_pPimpl->m_pubKey.SizeInBytes());
	}

	bool DHChachaFilter::ReceiveConnect(Buffer::Reader* apBuffer)
	{
		CryptoPP::SecByteBlock sharedSecret(m_pPimpl->m_dh.AgreedValueLength());
		CryptoPP::SecByteBlock pubKey(m_pPimpl->m_dh.PublicKeyLength());

		if (!apBuffer->ReadBytes((CryptoPP::byte*)pubKey, pubKey.SizeInBytes()))
			return false;

		if (!m_pPimpl->m_dh.Agree(sharedSecret, m_pPimpl->m_priKey, pubKey))
			return false;

		CryptoPP::SecByteBlock key(CryptoPP::SHA256::DIGESTSIZE);
		CryptoPP::SecByteBlock iv(CryptoPP::BLAKE2b::DIGESTSIZE);

		CryptoPP::SHA256().CalculateDigest(key, sharedSecret, sharedSecret.SizeInBytes());
		CryptoPP::BLAKE2b().CalculateDigest(iv, sharedSecret, sharedSecret.SizeInBytes());

		std::copy(iv.BytePtr(), iv.BytePtr() + std::size(m_iv), std::begin(m_iv));

		m_pPimpl->m_cipher.SetKeyWithIV(key.BytePtr(), key.SizeInBytes(), iv.BytePtr());

		return true;
	}

	bool DHChachaFilter::PreSend(Buffer::Writer* apBuffer, uint32_t aSequenceNumber)
	{
		(void)apBuffer;
		(void)aSequenceNumber;

		return true;
	}

	bool DHChachaFilter::PostSend(uint8_t* apPayload, size_t aLength, uint32_t aSequenceNumber)
	{
		// Operation is reversible
		return PreReceive(apPayload, aLength, aSequenceNumber);
	}

	bool DHChachaFilter::PreReceive(uint8_t* apPayload, size_t aLength, uint32_t aSequenceNumber)
	{
		std::array<uint8_t, 24> iv;

		std::copy(std::begin(m_iv), std::end(m_iv), std::begin(iv));

		uint8_t* pSequenceAsBytes = (uint8_t*)& aSequenceNumber;
		std::copy(pSequenceAsBytes, pSequenceAsBytes + 4, std::begin(iv) + std::size(m_iv));

		m_pPimpl->m_cipher.Resynchronize(iv.data(), std::size(iv));
		m_pPimpl->m_cipher.ProcessData(apPayload, apPayload, aLength);

		return true;
	}

	void DHChachaFilter::GenerateKeys()
	{
		CryptoPP::AutoSeededX917RNG<CryptoPP::AES> rng;

		m_pPimpl->m_dh.GenerateKeyPair(rng, m_pPimpl->m_priKey, m_pPimpl->m_pubKey);
	}
}