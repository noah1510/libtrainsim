#pragma once

#include "imguiHandler.hpp"

#include "dimensions.hpp"

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <tuple>
#include <vector>
#include <sstream>

namespace libtrainsim{
    namespace Video{

        /**
         * @brief a class to handle all kinds of textures.
         * 
         * This class is an abstraction on OpenGl textures.
         * It allows the creation of textures from nothing or by loading
         * an image file from a given path.
         * 
         * In addition to that it allows to easily create framebuffers 
         * which have a single texture attachment. This makes it surprisingly
         * simple to render into a texture.
         * 
         * There is also a function to display the texture as an ImGui Image.
         */
        class texture{
        private:
            /**
             * @brief the name of the texture
             */
            std::string name;
            
            /**
             * @brief the opengl texture id
             */
            unsigned int textureID = 0;
            
            /**
             * @brief the size of the image
             */
            dimensions imageSize = {0.0f,0.0f};
            
            /**
             * @brief a Mutex to control access to the texture
             */
            std::shared_mutex acessMutex;
            
            /**
             * @brief true if this texture has a framebuffer attached
             */
            bool framebufferMode = false;
            
            /**
             * @brief the opengl id of the framebuffer
             */
            unsigned int FBO = 0;
        public:
            /**
             * @brief create an empty texture
             */
            texture();
            
            /**
             * @brief create an empty texture with a name
             * 
             * @throws std::invalid_argument if the name is invalid
             * 
             * @param _name the name of the texture
             */
            texture(const std::string& _name);
            
            /**
             * @brief load a texture from a file
             * 
             * @throws std::invalid_argument if the path is empty
             * @throws std::runtime_error if the image cannot be loaded
             * @throws std::runtime_error if the image cannot be converted to rgba
             * 
             * @param URI the path to the texture
             */
            texture(const std::filesystem::path& URI);
            
            /**
             * @brief destroys the texture and frees the memory on the gpu
             */
            ~texture();
            
            /**
             * @brief get the dimensions of this texture
             */
            const dimensions& getSize() noexcept;
            
            
            /**
             * @brief get the opengl texture id of this texture
             */
            unsigned int getTextureId() noexcept;
            
            /**
             * @brief get the opengl framebuffer id of this texture
             */
            unsigned int getFBO() const noexcept;
            
            /**
             * @brief get the name of the texture
             */
            const std::string& getName() noexcept;
            
            
            /**
             * @brief update the data of the image from ram
             * 
             * @throws std::invalid_argument if there is less data than expected
             */
            void updateImage(const std::vector<uint8_t>& data, const dimensions& newSize);
            
            /**
             * @brief update the data of the image from ram
             */
            void updateImage(const uint8_t* data, const dimensions& newSize);
            
            /**
             * @brief bind the texture to a texture unit for use in shaders
             * 
             * This binds the texture to a texture unit so that shaders can
             * read from this texture.
             * 
             * @note check imguiHandler::getMaxTextureUnits() to see how many texture units are supported
             * 
             * @throw std::invalid_argument if the given unit is larger than the supported amount of texture units
             * 
             * @param unit The texture unit this texture will be bound to
             */
            void bind(unsigned int unit);
            
            /**
             * @brief change the size of the framebuffer
             * 
             * @throws std::invalid_argument if the texture is not in framebuffer mode
             */
            void resize(const dimensions& newSize);
            
            /**
             * @brief put the texture into framebuffer mode
             * 
             * @details this function allows using this texture as framebuffer.
             * This allows this texture to be rendered into.
             * 
             * @note if the texture is already in framebuffer mode this function will call resize
             * 
             * @throws std::nested_exception if the framebuffer cannot be created
             * @throws std::nested_exception if the framebuffer size cannot be updated
             * 
             * @param framebufferSize The (new) size of the framebuffer 
             */
            void createFramebuffer(const dimensions& framebufferSize);
            
            /**
             * @brief returns true if the texture is in framebuffer mode
             */
            bool hasFramebuffer() const noexcept;
            
            /**
             * @brief load the framebuffer to make it the current render target
             * 
             * @throws std::invalid_argument if the texture is not in framebuffer mode
             */
            void loadFramebuffer();
            
            /**
             * @brief display the texture as an image in the currently active imgui window
             * 
             * @param displaySize the size in pixel the image should have when being displayed
             */
            void displayImGui(const dimensions& displaySize = {0.0f,0.0f});
            
            /**
             * @brief get a projection matrix for the current framebuffer
             */
            glm::mat4 getProjection() noexcept;
            
            /**
             * @brief get a reference to the accessMutex
             * 
             * This allows locking the functions from the outside when more low level
             * operations are being made with the texture or framebuffer id.
             */
            std::shared_mutex& getMutex() noexcept;
        };
    }
}
