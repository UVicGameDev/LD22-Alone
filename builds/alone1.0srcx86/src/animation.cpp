#include "animation.hpp"
#include <cassert>

namespace alone
{
	Animation::Animation(AnimData& anim_data)
		: anim_data_(anim_data),
		timeline_(0),
		fps_(anim_data.fps()),
		current_frame_(0),
		total_frames_(anim_data_.total_frames()),
		playing_(false)
	{
		sprite_.SetTexture(anim_data.texture());
		update_sprite_to_frame();
	}
	bool Animation::playing() const
	{
		return playing_;
	}
	void Animation::play(const std::string& sequence_name)
	{
		const AnimSequence& seq(anim_data_.sequence(sequence_name));
		loop_start_ = seq.start();
		loop_end_ = seq.end();
		play(loop_start_);
	}
	void Animation::play(int frame)
	{
		playing_ = true;
		current_frame_ = frame;
		timeline_ = frame_to_time(frame);
		update_sprite_to_frame();
	}
	void Animation::hold(const std::string& sequence_name)
	{
		const AnimSequence& seq(anim_data_.sequence(sequence_name));
		loop_start_ = seq.start();
		loop_end_ = seq.end();
		hold(loop_start_);
	}
	void Animation::hold(int frame)
	{
		playing_ = false;
		current_frame_ = frame;
		timeline_ = frame_to_time(frame);
		update_sprite_to_frame();
	}
	sf::Vector2i Animation::point_relative(const std::string& point_name) const
	{
		return static_cast<sf::Vector2i>(position()) + anim_data_.point(point_name);
	}
	sf::Vector2i Animation::point(const std::string& point_name) const
	{
		return anim_data_.point(point_name);
	}
	sf::IntRect Animation::rect_relative(const std::string& rect_name) const
	{
		sf::IntRect rect = anim_data_.rect(rect_name);
		return sf::IntRect(static_cast<sf::Vector2i>(position()) + sf::Vector2i(rect.Left,rect.Top),sf::Vector2i(rect.Width,rect.Height));
	}
	sf::IntRect Animation::rect(const std::string& rect_name) const
	{
		return anim_data_.rect(rect_name);
	}
	sf::IntRect Animation::anim_rect_relative() const
	{
		return sf::IntRect(
			static_cast<int>(position().x),
			static_cast<int>(position().y),
			width(),
			height()
			);
	}
	sf::IntRect Animation::anim_rect() const
	{
		return sf::IntRect(0, 0, width(), height());
	}
	sf::Vector2f Animation::center_relative() const
	{
		return sf::Vector2f(position().x + width()/2.0f, position().y + height()/2.0f);
	}
	sf::Vector2f Animation::center() const
	{
		return sf::Vector2f(width()/2.0f,height()/2.0f);
	}
	float Animation::constant(const std::string& constant_name) const
	{
		return anim_data_.constant(constant_name);
	}
	int Animation::width() const
	{
		return static_cast<int>(sprite_.GetSubRect().Width * scale().x);
	}
	int Animation::height() const
	{
		return static_cast<int>(sprite_.GetSubRect().Height * scale().y);
	}
	sf::Uint32 Animation::fps() const
	{
		return fps_;
	}
	void Animation::set_fps(sf::Uint32 fps)
	{
		fps_ = fps;
	}
	void Animation::set_position(const sf::Vector2f& p)
	{
		sprite_.SetPosition(p);
	}
	sf::Vector2f Animation::position() const
	{
		return sprite_.GetPosition() - sf::Vector2f();
	}
	void Animation::set_alpha(sf::Uint8 a)
	{
		sf::Color new_color(sprite_.GetColor());
		new_color.a = a;
		sprite_.SetColor(new_color);
	}
	sf::Uint8 Animation::alpha() const
	{
		return sprite_.GetColor().a;
	}
	void Animation::set_tint(const sf::Color& color)
	{
		sprite_.SetColor(sf::Color(color.r,color.g,color.b,alpha()));
	}
	const sf::Color& Animation::tint() const
	{
		return sprite_.GetColor();
	}
	const sf::Vector2f& Animation::scale() const
	{
		return sprite_.GetScale();
	}
	void Animation::set_scale(const sf::Vector2f& scale)
	{
		set_scale(scale.x,scale.y);
	}
	void Animation::set_scale(float w, float h)
	{
		assert( w >= 0 && h >= 0 );
		sprite_.SetScale(w,h);
	}
	void Animation::fliph(bool flipped)
	{
		sprite_.FlipX(flipped);
	}
	void Animation::flipv(bool flipped)
	{
		sprite_.FlipX(flipped);
	}
	void Animation::update(sf::Uint32 dt)
	{
		if(playing_)
		{
			int previous_frame = current_frame_;
			timeline_ += dt;
			if(timeline_ >= frame_to_time(current_frame_+1))
			{
				++current_frame_;
			}
			if(current_frame_ > loop_end_)
			{
				current_frame_ = loop_start_;
				timeline_ = frame_to_time(loop_start_);
			}
			if(previous_frame != current_frame_)
			{
				update_sprite_to_frame();
			}
		}
	}
	void Animation::draw(sf::RenderTarget& target)
	{
		target.Draw(sprite_);
	}
	sf::Uint32 Animation::frame_to_time(int frame) const
	{
		assert(fps() != 0);
		return static_cast<sf::Uint32>((1000*(static_cast<float>(frame)/fps())));
	}
	void Animation::update_sprite_to_frame()
	{
		int framewidth = sprite_.GetTexture()->GetWidth() / total_frames_;
		int frameheight = sprite_.GetTexture()->GetHeight();
		sf::IntRect clip(current_frame_ * framewidth , 0, framewidth, frameheight);
		sprite_.SetSubRect(clip);
	}
}
