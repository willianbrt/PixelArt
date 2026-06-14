#include "RendererPipeline.h"

RendererPipeline::RendererPipeline(){
}
void RendererPipeline::registerPass(IRenderPass* pass){
    passes.push_back(pass);
}

void RendererPipeline::render(){
    for(int i = 0; i < passes.size(); i++){
        passes[i]->render();
    }
}
