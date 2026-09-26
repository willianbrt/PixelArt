
export function repository(){
    const req = indexedDB.open("editor", 1);
    req.onupgradeneeded = (e) => {
        const db = e.target.result;

        if (!db.objectStoreNames.contains("pattern")) {
            const s =db.createObjectStore("pattern", {
                keyPath: "id",
                autoIncrement: true
            });
            s.createIndex("buffer", "buffer", {unique:true});
        }

        if (!db.objectStoreNames.contains("palette")) {
            const s = db.createObjectStore("palette", {
                keyPath: "id",
                autoIncrement: true
            });
            s.createIndex('name', 'name');
        }

        if (!db.objectStoreNames.contains("autosave")) {
            const s =db.createObjectStore("autosave", {
                keyPath: "id",
                autoIncrement: true
            });
        }
    };
    return new Promise((resolve, reject) => {
        req.onsuccess = () => {
            const db = req.result;
            function table(storeName) {
                return {
                    getAll: (offset=null, limit=null) => new Promise((resolve, reject) => {
                        const tx = db.transaction(storeName, "readonly");
                        const store = tx.objectStore(storeName);

                        if(offset == null || limit == null){
                            const req = store.getAll();
                            req.onsuccess = (e) => {resolve(req.result);}
                            req.onerror = (e) => {reject(req.error);}
                            return;
                        }
                        
                        let skipped = 0;
                        const result = [];
                        
                        const req = store.openCursor();
                        req.onsuccess = (e) => {
                            const cursor = e.target.result;

                            if (!cursor) {
                                resolve(result);
                                return;
                            }

                            if (skipped < offset) {
                                skipped++;
                                cursor.continue();
                                return;
                            }

                            if (result.length < limit) {
                                result.push(cursor.value);
                                cursor.continue();
                            } else {
                                resolve(result);
                            }
                        };

                        req.onerror = () => reject(req.error);
                    }),
                    size: () => new Promise((resolve, reject) => {
                        const tx = db.transaction(storeName, "readonly");
                        const store = tx.objectStore(storeName);

                        const req = store.count();
                        req.onsuccess = () => resolve(req.result);
                        req.onerror = () => reject(req.error);
                    }),

                    getById: (id) => new Promise((resolve, reject) => {
                        const tx = db.transaction(storeName, "readonly");
                        const store = tx.objectStore(storeName);
                        const req = store.get(id);

                        req.onsuccess = () => resolve(req.result);
                        req.onerror = () => reject(req.error);
                    }),
                    findBy: (index, value) => new Promise((resolve, reject) => {
                        const tx = db.transaction(storeName, "readonly");
                        const store = tx.objectStore(storeName);
                        const indexStore = store.index(index);
                        const req = indexStore.get(value);

                        req.onsuccess = () => resolve(req.result);
                        req.onerror = () => reject(req.error);
                    }),

                    put: (object) => new Promise((resolve, reject) => {
                        const tx = db.transaction(storeName, "readwrite");
                        const store = tx.objectStore(storeName);
                        const req = store.put(object);

                        req.onsuccess = () => resolve(req.result);
                        req.onerror = () => reject(req.error);
                    }),

                    delete: (id) => new Promise((resolve, reject) => {
                        const tx = db.transaction(storeName, "readwrite");
                        const store = tx.objectStore(storeName);
                        const req = store.delete(id);

                        req.onsuccess = () => resolve();
                        req.onerror = () => reject(req.error);
                    })
                };
            }
            resolve({table});
        };
        req.onerror = () => reject(req.error);
    });
}