#include "../Header/BloomEffect.h"


BloomEffect::BloomEffect()
	: PostEffect()
	, mShaders()
	, mBrightnessTexture()
	, mFirstPassTextures()
	, mSecondPassTextures()
{
	// Vertex Shader, Fragment Shader
	mShaders.load(Shaders::ID::BrightnessPass,
		"Media/Shaders/Fullpass.vert", "Media/Shaders/Brightness.frag");
	mShaders.load(Shaders::ID::DownSamplePass,
		"Media/Shaders/Fullpass.vert", "Media/Shaders/DownSample.frag");
	mShaders.load(Shaders::ID::GaussianBlurPass,
		"Media/Shaders/Fullpass.vert", "Media/Shaders/GaussianBlur.frag");
	mShaders.load(Shaders::ID::AddPass,
		"Media/Shaders/Fullpass.vert", "Media/Shaders/Add.frag");
}

void BloomEffect::apply(const sf::RenderTexture& input, sf::RenderTarget& output)
{
	prepareTextures(input.getSize());

	filterBright(input, mBrightnessTexture);

	downsample(mBrightnessTexture, mFirstPassTextures[0]);
	blurMultipass(mFirstPassTextures);

	downsample(mFirstPassTextures[0], mSecondPassTextures[0]);
	blurMultipass(mSecondPassTextures);

	add(mFirstPassTextures[0], mSecondPassTextures[0], mFirstPassTextures[1]);
	mFirstPassTextures[1].display();
	add(input, mFirstPassTextures[1], output);
}



	typedef std::array<sf::RenderTexture, 2> RenderTextureArray;

void BloomEffect::prepareTextures(sf::Vector2u size)
{
	if (mBrightnessTexture.getSize() != size)
	{
		mBrightnessTexture.resize(size);
		mBrightnessTexture.setSmooth(true);

		sf::Vector2u halfSize = { size.x / 2, size.y / 2 };
		mFirstPassTextures[0].resize(halfSize);
		mFirstPassTextures[0].setSmooth(true);
		mFirstPassTextures[1].resize(halfSize);
		mFirstPassTextures[1].setSmooth(true);

		sf::Vector2u quarterSize = { size.x / 4, size.y / 4 };
		mSecondPassTextures[0].resize(quarterSize);
		mSecondPassTextures[0].setSmooth(true);
		mSecondPassTextures[1].resize(quarterSize);
		mSecondPassTextures[1].setSmooth(true);
	}
}

void BloomEffect::filterBright(const sf::RenderTexture& input, sf::RenderTexture& output)
{
	sf::Shader& brightness = mShaders.get(Shaders::ID::BrightnessPass);

	brightness.setUniform("source", input.getTexture());
	applyShader(brightness, output);
	output.display();
}

void BloomEffect::blurMultipass(RenderTextureArray& renderTextures)
{
	sf::Vector2u textureSize = renderTextures[0].getSize();

	for (size_t count = 0; count < 2; ++count)
	{ // The blur function ping-pongs the two renderTextures[0] and [1]
		// Vertical blur
		blur(renderTextures[0], renderTextures[1], 
			sf::Vector2f(0.f, 1.f / textureSize.y));
		// Horizontal blur
		blur(renderTextures[1], renderTextures[0],
			sf::Vector2f(1.f / textureSize.x, 0.f));
	}
}

void BloomEffect::blur(const sf::RenderTexture& input, sf::RenderTexture& output,
	sf::Vector2f offsetFactor)
{
	sf::Shader& gaussianBlur = mShaders.get(Shaders::ID::GaussianBlurPass);

	gaussianBlur.setUniform("source", input.getTexture());
	gaussianBlur.setUniform("offsetFactor", offsetFactor);
	applyShader(gaussianBlur, output);
	output.display();
}

void BloomEffect::downsample(const sf::RenderTexture& input,
	sf::RenderTexture& output)
{
	sf::Shader& downSampler = mShaders.get(Shaders::ID::DownSamplePass);

	downSampler.setUniform("source", input.getTexture());
	downSampler.setUniform("sourceSize", sf::Vector2f(input.getSize()));
	applyShader(downSampler, output);
	output.display();
}

void BloomEffect::add(const sf::RenderTexture& source,
	const sf::RenderTexture& bloom, sf::RenderTarget& output)
{
	sf::Shader& adder = mShaders.get(Shaders::ID::AddPass);

	adder.setUniform("source", source.getTexture());
	adder.setUniform("bloom", bloom.getTexture());
	applyShader(adder, output);
}

	
