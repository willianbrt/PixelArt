export function modal(options){
    let { modalId, triggerId } = options;
    let btnOpenModal = document.getElementById(triggerId);
    let modalElem = document.getElementById(modalId);

    btnOpenModal?.addEventListener("click", open);

    function open(){
        modalElem.classList.toggle("hidden", false);

        let abort = new AbortController();
        
        abort.signal.addEventListener("abort", close);

        window.addEventListener("mousedown", function(){
            if (btnOpenModal.contains(event.target) || modalElem.contains(event.target)){
                return;
            }
            abort.abort();
        }, { signal:abort.signal });
    }
    function close(){
        modalElem.classList.toggle("hidden", true);
    }
    return Object.assign(open, close);
}
    


    let tab = document.querySelector(".tab");
    let nav = tab.querySelector(".tab-nav");
    let navItems = nav.querySelectorAll(".tab-nav-item");
    navItems.forEach((navItem)=>{
        navItem.onclick = ()=>{
            nav.querySelector(".tab-nav-item.active")?.classList.remove("active");
            tab.querySelector(".tab-items .tab-select.active")?.classList.remove("active");

            navItem.classList.add("active");
            let tabPane = document.getElementById(navItem.dataset.pageId);
            tabPane?.classList.add("active");
        }
    });

    let tabFill = document.querySelector("#tab-fill");
    let mode = {
        "solid": ()=>{
            let lastActivePane = tabFill.querySelector(".tab-select.active");
            lastActivePane?.classList.remove("active");
            
            let activePane = tabFill.querySelector("#pane-solid");
            activePane.classList.add("active");
        },
        "stamp": ()=>{
            let lastActivePane = tabFill.querySelector(".tab-select.active");
            lastActivePane?.classList.remove("active");
            
            let activePane = tabFill.querySelector("#pane-stamp");
            activePane.classList.add("active");
        },
        "pattern": ()=>{
            let lastActivePane = tabFill.querySelector(".tab-select.active");
            lastActivePane?.classList.remove("active");

            let activePane = tabFill.querySelector("#pane-pattern");
            activePane.classList.add("active");
        },
    };

let panePattern = document.querySelector("#pattern");
let selectModeBrush = document.querySelector("#mode-brush");
selectModeBrush.onchange = (e)=>{ mode[e.srcElement.value]() };
mode[selectModeBrush.value]();

export async function carousel(options){
    let { id, empty, _index=0, limit=5, offset, search, element } = options;
    let data, start, end, numberPages;
    changeIndex(_index);

    let carouselElem = document.querySelector(id);

    let pages = document.createElement("div");
    pages.className = "list-pattern";

    let btnNext = document.createElement("span");
    btnNext.className = "material-icons";
    btnNext.innerText = "arrow_right";
    btnNext.addEventListener("click", next);
    
    let btnPrevious = document.createElement("span");
    btnPrevious.className = "material-icons";
    btnPrevious.innerText = "arrow_left";
    btnPrevious.addEventListener("click", previous);

    let controlPageList = document.createElement("div");
    controlPageList.className = "carousel-list-index";
    
    let control = document.createElement("div");
    control.className = "carousel-pages";
    control.append(btnPrevious);
    control.append(controlPageList);
    control.append(btnNext);
    
    function next(e){
        if(_index >= numberPages-1)  return; 
        changeIndex(_index+1);
    }
    function previous(){
        if(_index <= 0) return;
        changeIndex(_index-1);
    }

    function createBtnIndex(i){
        let btnIndex = document.createElement("span");
        btnIndex.className = "btn-carousel";
        
        if(_index == i){
            btnIndex.classList.add("active");
        }
        
        btnIndex.dataset.index = i;
        btnIndex.addEventListener("click", function(e){
            changeIndex(i);
        });
        return btnIndex;
    }

    async function changeIndex(index){
        data = await search(index, limit);
            
        numberPages = Math.ceil(data.dataLength/limit);

        btnNext.classList.toggle("disabled", _index >= numberPages-1);
        btnPrevious.classList.toggle("disabled", _index <= 0);

        control.querySelector('.btn-carousel.active')?.classList.remove('active');
        control.querySelector(`.btn-carousel[data-_index="${_index}"]`)?.classList.add("active");
        if(numberPages > 0){
            _index = index;

            controlPageList.innerHTML = "";
            carouselElem.append(pages);
            carouselElem.append(control);

            let limitPages = Math.min(5, numberPages);

            start =  Math.max((index-parseInt(limitPages/2)), 0);
            end = Math.min(start + limitPages, numberPages);

            for(let i = start; i < end; i++ ){
                controlPageList.append(createBtnIndex(i));
            }
        }else{
            _index = 0;
            pages.append(empty());
        }

        pages.innerHTML = "";
        
        for(let i = 0; i < Math.min(limit, data.data.length); i++ ){
            pages.append(element.call(pages, data.data[i]));
        }
    }
    return {
        update: ()=> changeIndex(_index)
    }
}