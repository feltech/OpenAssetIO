# Cursor object sketches

## Prior art

General notes:
- Seems common to set a background batch size, but retrieve records one at a time.


CloudFoundry (JSON REST):
```
page: int
per_page: int
order_by: str
def total_results() -> int: ...
def total_pages() -> int: ...
def first(): ..
def last(): ...
def next(): ...
def previous() -> Union[None, List]: ...
```

J2SE ResultSet

```
def absolute(row: int): ...
def relative(rows: int): ...
def setFetchSize(rows: int): ...
def setFetchDirection(direction: int): ...
def next(): ...  # Move _one_ row
def previous(): ...
def first(): ...
def last(): ...
def isFirst(): ...
def isLast(): ...
def get<type>(columnIndex: int): ...  # value at current row at column columnIndex as <type>.
def getRow(): ... # current row number
def getMetaData(): ... # column metadata
def close(): ... # pre-emptively close the connection
```

MongoDB (mongosh)

https://www.mongodb.com/docs/manual/reference/method/js-cursor/

 ```
def batchSize(size: int): ...  # Num docs per network message
def close(): ...
def collation(collation): ... # Sets collation: locale, numericOrdering, caseFirst, etc
def comment(string: str): ... # Comment to attach to queries for traceability in logs
def forEach(callback): ...  # Callback sig: `callback(document)`
def hasNext(): ...
def min / hint / max (index): ...  # Hint indexed fields to improve perf
def count(): ... # Lots of caveats
def itcount(): ... # Actually executes query and loops the results to count them
def isExhausted(): ...
def limit(int): ...
def map(callback) -> Cursor: ...  # Can call .toArray() on returned cursor object
def toArray(): ...  # Exhausts the cursor
```

MongoDB (Motor Python async API)

https://motor.readthedocs.io/en/stable/api-tornado/cursors.html#motor.motor_tornado.MotorCursor
```
def clone(): ...
def close(): ...
def distinct(key: str) -> List: ...
def each(callback): ...  # Async. Callback sig: `each(result, error)`
def min / hint / max (index): ...  # Hint indexed fields to improve perf
def limit(limit: int): ...  # Hard limit
def next(): ...
def rewind(): ...
def skip(skip: int): ...
def sort(key_or_list): ...
async def to_list(length: int): ...
def where(code): ...  # Apply filter
def alive(): ... # Potentially more data, esp. tailable cursors
def fetch_next(): ...
def next_object(): ...
for ... in ...
```

MongoDB (Pymongo)

```
def batch_size(batch_size: int): ...   # Num docs per network message
def __getitem__(index: int|slice): ... #  E.g. cursor[3:45] - uses offset + limit
def next() -> Document: ...  # Advance cursor and get next document
```

Ruby on Rails

```
def in_batches(of=1000,  start=None, finish=None, order="asc") -> List: ...
```

IndexedDB

```
def advance(value: int): ...
def continue(key: str = None): ...   # Advance cursor to matching `key`, or next record
def value(): ...
```

ADO.NET

https://learn.microsoft.com/en-us/office/client-developer/access/desktop-database-reference/recordset-properties-methods-and-events-ado

```
def MoveFirst(): ...
def MoveLast(): ...
def MoveNext(): ...
def MovePrevious(): ...
def Move(int): ...
def Seek(KeyValues, SeekOption): ...  # Seek through recordset to matching record
def GetRows(Rows: int, Start: None, Fields: None)
```

## Possible implementations

TODO:
- Resizable UI

### Version dropdown

#### Return a list

```python
class EntityVersionsPageIter:
    def next(self, pageSize) -> List[VersionInfo]: ...
```
```python
pages = manager.entityVersions("asset://my/thing", context)

kPageSize = 20
first_page = pages.next(kPageSize)

def update_combo_box_options(options):
    if len(options) % kPageSize == 1:  # TODO: doesn't work.
        options += ["more"]
    combo_box.set_options(options)

update_combo_box_options(first_page)
render_version_info(first_page[0])

def on_option_chosen_cb(idx, value):
    if value == "more":
        current_options = combo_box.get_options()[:-1]  # Remove "more"
        next_page = pages.next(kPageSize)
        options = current_options + next_page
        update_combo_box_options(options)
    else:
        render_version_info(value)

combo_box.on_option_chosen(on_option_chosen_cb)
```

#### List provided to a callback

```python
EntityVersionsSuccessCallback = Callable[[List[EntityReference]], None]
EntityVersionsErrorCallback = Callable[[PageError], None]

class EntityVersionsPageIter:
    def next(self, pageSize,
             successCb: EntityVersionsSuccessCallback,
             errorCb: EntityVersionsErrorCallback): ...
```
```python
pages = manager.entityVersions("asset://my/thing", context)

kPageSize = 20


def update_combo_box_options(options):
    if len(options) == kPageSize:   # TODO: doesn't work.
        options += ["more"]
    combo_box.set_options(options)


def on_page_success(entity_refs):
    current_options = combo_box.get_options()[:-1]  # Remove "more"
    options = current_options + next_page
    update_combo_box_options(options)


def on_page_error(page_error):
    raise Exception("Failed to get versions: %s" % str(page_error))


def on_option_chosen_cb(idx, value):
    if value == "more":
        pages.next(kPageSize, on_page_success, on_page_error)
    else:
        render_version_info(value)


pages.next(kPageSize, on_page_success, on_page_error)
# Assumes synchronous `next` call, above.
render_version_info(first_page[0])

combo_box.on_option_chosen(on_option_chosen_cb)
```

#### Return a single record

```python
class RelatedEntitiesPageIter:
    def __init__(self, pageSize): ...
    def next(self) -> VersionInfo: ...
```
```python
kPageSize = 20

cursor = manager.entityVersions("asset://my/thing", context, kPageSize)

first_option = cursor.next()
combo_box.add_option(first_option)

def add_more_combo_box_options():
    while option := cursor.next():
        combo_box.add_option(option)
        if combo_box.size() % kPageSize == 0:
            combo_box.add_option("more")
            break

add_more_combo_box_options()
render_version_info(first_option)

def on_option_chosen_cb(idx, value):
    if value == "more":
        combo_box.remove_option(-1)  # Remove "more"
        add_more_combo_box_options()
    else:
        render_version_info(value)

combo_box.on_option_chosen(on_option_chosen_cb)
```

#### Single record provided to a callback

```python
EntityVersionsSuccessCallback = Callable[[List[EntityReference]], None]
EntityVersionsErrorCallback = Callable[[PageError], None]

class EntityVersionsPageIter:
    def next(self, pageSize,
             successCb: EntityVersionsSuccessCallback,
             errorCb: EntityVersionsErrorCallback): ...
```
```python
pages = manager.entityVersions("asset://my/thing", context)

kPageSize = 20


def update_combo_box_options(options):
    if len(options) == kPageSize:
        options += ["more"]
    combo_box.set_options(options)


def on_page_error(page_error):
    raise Exception("Failed to get versions: %s" % str(page_error))


def on_option_chosen_cb(idx, value):
    if value == "more":
        options = combo_box.get_options()[:-1]  # Remove "more"

        def on_element_success(entity_ref):
            options.append(entity_ref)

        pages.next(kPageSize, on_element_success, on_page_error)
        update_combo_box_options(options)
    else:
        render_version_info(value)


options = combo_box.get_options()[:-1]  # Remove "more"

def on_element_success(entity_ref):
    options.append(entity_ref)

pages.next(kPageSize, on_element_success, on_page_error)
update_combo_box_options(options)
# Assumes synchronous `next` call, above.
render_version_info(first_page[0])

combo_box.on_option_chosen(on_option_chosen_cb)
```

### Versions panel

#### List provided to a callback

```cpp
class ShowVersionsPanelVectorCallback {
  Q_OBJECT
public:
  ShowVersionsPanel(QPointer<QPushButton> nextPageButton,
                    const openassetio::Manager &manager,
                    const openassetio::Context &context,
                    const openassetio::EntityReference &initialEntityReference,
                    size_t page_display_size = 10, QObject *parent = nullptr)
      : m_nextPageButton(nextPageButton),
        m_page_display_size(page_display_size), m_manager(manager),
        m_context(context), m_ui(new Ui::ShowVersionPanel) {

    m_ui->setupUI(this);

    QObject::connect(&m_nextPageButton, &Button::clicked, this, [this]() {
      this->m_cursor.nextPage(&ShowVersionsPanel::_SetEntityDisplay)
    });

    SetEntity(initialEntityRef)
  }

  void SetEntity(EntityReference newEntityRef) {
    m_ui->listDisplay->clear();
    m_cursor = manager.entityVersions(entityRef, context, m_page_display_size);
    m_cursor.nextPage(&ShowVersionsPanel::_SetEntityDisplay);
  }

private:
  Cursor<EntityVersions> m_cursor;
  QPointer<QPushButton> m_nextPageButton;
  size_t m_page_display_size;
  const openassetio::Manager &m_manager;
  const openassetio::Context &m_context;
  Ui::ShowVersionPanel *m_ui;

  void _SetEntityDisplay(std::vector<EntityVersions> &results) {
    std::vector<QString> resultStrings;
    std::transform(
        results.cbegin(), results.cend(), std::back_inserter(resultStrings),
        [](const EntityVersion version) { return version.versionString(); });

    m_ui->displayPanel->setData(resultStrings);
  }
}

```

### Batch processing version info

#### Return a list

```c++
// Return Vector - Batch Processing
void ReturnVectorBatchProcess() {
  std::vector<ProcessedVersion> results;

  constexpr page_size = 1000;
  auto pager = entityVersions(entityRef, context, page_size);

  std::vector<EntityVersion> currentPage = pager.nextPage();
  std::vector<ProcessedVersion> processedResults;
  while (currentPage.size() > 0) {
    processedResults.resize(processedResults.size() + currentPage.size())
    std::transform(std::execution::par, currentPage.cbegin(), currentPage.cend(), results.begin(),
    [](const EntityVersions& entityVersion){
      return ProcessedVersion(entityVersion);
    }
    try {
      currentPage = pager.nextPage();
    } catch(...)
    {
      logError("Could not retrieve next page, aborting processing");
      results.clear();
    }
  }
}
```

#### List provided to a callback

```c++
void CallbackVectorBatchProcess() {

  bool finished = false;
  std::vector<ProcessedVersion> results;

  auto page_callback =
      [&results, &finished](std::vector<EntityVersion> entityVersionsPage) {
        if (entityVersionsPage.size() == 0) {
          finished = true;
          return;
        }

        results.resize(results.size() + entityVersions.size());

        std::transform(std::execution::par, entityVersionsPage.cbegin(),
                       entityVersionsPage.cend(), results.begin(),
                       [](const EntityVersions &entityVersion) {
                         return ProcessedVersion(entityVersion);
                       });
      }

  auto pager = entityVersions(entityRef, context, page_size);
  while (finished == false) {
    try {
      pager.nextPage(page_callback);
    } catch (...) {
      logError("Could not retrieve next page, aborting processing");
      results.clear();
    }
  }
}
```

#### Return a single record

```c++
// Return Singular - Batch Processing (Requires number of elements)
void ReturnSingularBatchProcess() {
  std::vector<ProcessedVersion> results;

  constexpr size_t page_size_hint = 1000;
  //"pager", Are pages a helpful concept when doing singular?
  auto pager = entityVersions(entityRef, context, page_size_hint);

  std::vector<ProcessedVersion> processedResults;
  processedResults.reserve(
      pager.NumElements()); // Necessary?!
                            // The way to do this otherwise would be
                            // join-and-stitch with a promise interface, which
                            // is probably more standatd

  auto process_func = [&processedResults](EntityVersion version, size_t index) {
    processedResults[index] = ProcessedVersion(version);
  };

  std::vector<std::jthread> workers;
  workers.reserve(pager.NumElements());
  std::optional<EntityVersion> curItem = pager.nextElement();
  size_t index;
  while (curItem.hasValue()  {
    try {
      workers.emplace_back(process_func, curItem.Value(), index);
      ++index;
      curItem = pager.nextElement();
    } catch (...) {
      logError("Could not advance to next element, aborting processing");
      results.clear();
    }
  }

  for (auto& worker : workers)
   worker.join();
}
```

#### Single record provided to a callback

```c++
void CallbackSingularBatchProcess() {
  std::vector<ProcessedVersion> results;

  constexpr size_t page_size_hint = 1000;
  //"pager", Are pages a helpful concept when doing singular?
  auto pager = entityVersions(entityRef, context, page_size_hint);

  std::vector<ProcessedVersion> processedResults;
  processedResults.resize(pager.NumElements()); // Neccesarry!

  bool isFinished = true;

  auto process_func = [&processedResults](EntityVersion version, size_t index) {
    processedResults[index] = ProcessedVersion(version);
  };

  std::vector<std::jthread> workers;

  auto callback = [&process_func, &isFinished](EntityVersion &version,
                                  size_t index) { // Callback must provide an
                                                  // index in singular callback?
    isFinished = false;
    workers.emplace_back(process_func, curItem.Value(), index);
  };

  workers.reserve(pager.NumElements());
  pager.nextElement(callback);
  while (!isFinished)  {
    try {
      isFinished = true;
      pager.nextElement(callback);
    } catch (...) {
      logError("Could not advance to next element, aborting processing");
      results.clear();
    }
  }

  for (auto& worker : workers)
    worker.join();
}
```

### Rendering AOVs

Rendering AOVs across multiple sub-processes, a page per process.

#### Return a list

```python
class RelatedEntitiesPageIter:
    def next(self, pageSize) -> List[EntityReferences]: ...
```
```python
relationship = ChildAOVRelationshipSpecification()

pages = manager.getWithRelationship("asset://my/render/bundle", relationship.traitsData(), context)

page_size = optimal_aovs_per_cpu()

with multiprocessing.Pool() as pool:
    while page := pages.next(page_size):
        pool.apply_async(render_aovs, (input_node, page))
```

#### List provided to a callback

```python
RelatedEntitiesSuccessCallback = Callable[[List[EntityReference]], None]
RelatedEntitiesErrorCallback = Callable[[PageError], None]

class RelatedEntitiesPageIter:
    def next(self, pageSize,
             successCb: RelatedEntitiesSuccessCallback,
             errorCb: RelatedEntitiesErrorCallback): ...
```
```python
pages = manager.getWithRelationship("asset://my/render/bundle", relationship.traitsData(), context)

kPageSize = 20

has_more_pages = True

with multiprocessing.Pool() as pool:

    def on_page_success(entity_refs):
        global has_more_pages
        has_more_pages = bool(entity_refs)
        if has_more_pages:
            pool.apply_async(render_aovs, (input_node, entity_refs))


    def on_page_error(page_error):
        raise Exception("Failed to get related entities: %s" % str(page_error))


    while has_more_pages:
        pages.next(kPageSize, on_page_success, on_page_error)
```

#### Return a single record

```python
class RelatedEntitiesPageIter:
    def __init__(self, pageSize): ...
    def next(self) -> EntityReference: ...
```
```python
relationship = ChildAOVRelationshipSpecification()

page_size = optimal_aovs_per_cpu()

cursor = manager.getWithRelationship(
    "asset://my/render/bundle", relationship.traitsData(), context, pageSize=page_size)

with multiprocessing.Pool() as pool:
    while page := [cursor.next() for _ in range(page_size)]:
        pool.apply_async(render_aovs, (input_node, page))
```

#### Single record provided to a callback

```python
pages = manager.getWithRelationship("asset://my/render/bundle", relationship.traitsData(), context)

kPageSize = 20

with multiprocessing.Pool() as pool:

    def on_page_error(page_error):
        raise Exception("Failed to get related entities: %s" % str(page_error))

    has_more_pages = True

    while has_more_pages:
        has_more_pages = False

        def on_element_success(entity_ref):
            global has_more_pages
            has_more_pages = True
            pool.apply_async(render_aovs, (input_node, entity_ref))

        pages.next(kPageSize, on_element_success, on_page_error)
```

```python
kPageSize = 20

cursor = manager.getWithRelationship("asset://my/render/bundle", relationship.traitsData(), context, kPageSize)

with multiprocessing.Pool() as pool:

    def on_page_error(page_error):
        raise Exception("Failed to get related entities: %s" % str(page_error))


    while cursor.has_more():

        def on_element_success(entity_ref):
            pool.apply_async(render_aovs, (input_node, entity_ref))

        cursor.next(on_element_success, on_page_error)
```

### Outcome so far

Abandon "Single record provided to a callback"


### Proposed interface

TODO:

* Sketch `entityVersions` / `getWithRelationships` esp. on manager side
  - REST API

Questions:

* Discourage keeping hold of `Context` and `HostSession`, since they are
mutable?
  - `Context` could be deep-copied.
  - Mutable `HostSession` might not matter, and is perhaps correct
    anyway.

*  Should copying of the `Manager` wrapper cursor be allowed (whilst
   copying of the underlying `ManagerInterface` cursor is not)? Probably
   not so important, since it's wrapped in a `shared_ptr` anyway, which
   is (obviously) copyable.

* Adapter design pattern vs. template method pattern (i.e. two classes
  vs. one class)?
  - Adapter is symetrical with Manager/ManagerInterface

* Naming things: PageIterator vs. Pager vs. Cursor?

* `Context` and `HostSession` passed to every back-end call because
    - Something something consistency
    - Limits the amount of state that the manager plugin has to store in
      the cursor.
        * But they have to store _some_ state anyway, so why not a bit more.
            - But they don't _have_ to store it in the cursor.

* Do we need to support multiple container types?  Is it OK to lock in
  `vector` as the only possible container type.
  - E.g. `std::map` with elements that have a customizable `<` operator.

* When does an invalid request throw?
  - Ideally on cursor creation, but adds complexity to manager and
    unwanted delays to host.
  - Should cursor creation be allowed to throw _as well as_ `nextPage`,
    for the same type of (validation) error.
  - Cursor creation function `noexcept`?

Decisions:
* `Context` created using `createChildContext`.


#### Return a list

* Error handling via exceptions.

ManagerInterface:

```c++
template <class Elem>
class PagerInterface {
 public:
    // Still need shared_ptr because Python.
    using Ptr = std::shared_ptr<PagerInterface<Elem>>;
    using Page = std::vector<Elem>;

    // Explicitly disallow copying.
    PagerInterface(const& PagerInterface) = delete;
    PagerInterface<Elem>& operator=(const PagerInterface<Elem>&) = delete;
    // Allow moving.
    PagerInterface(PagerInterface&&) noexcept = default;
    PagerInterface<Elem>& operator=(PagerInterface<Elem>&&) noexcept = default;
    // Virtual destructor communicates when query has finished.
    virtual ~PagerInterface() = default;

    // Really very useful in many workflows
    virtual bool hasMore();

    virtual Page nextPage(
            const ContextConstPtr& context,
            const HostSessionPtr& hostSession,
            std::size_t pageSize) = 0;
};
```

Manager:

```c++
template <class Elem>
class Pager {
 public:
    using Ptr = std::shared_ptr<Pager<Elem>>;
    using Page = PagerInterface<Elem>::Page;

    Pager(log::AuditorPtr auditor,
          ContextConstPtr context,  // <- manager.createChildContext(context)
          HostSessionPtr hostSession,
          PagerInterface<Elem>::Ptr PagerInterface)
     : auditor_{std::move(auditor)},
       context_{std::move{context}},
       hostSession_{std::move(hostSession)},
       PagerInterface_{std::move(PagerInterface)} { }

    Pager(const Pager&) = delete;
    Pager<Elem>& operator=(const Pager<Elem>&) = delete;
    Pager(Pager&&) noexcept = default;
    Pager<Elem>& operator=(Pager<Elem>&&) noexcept = default;
    ~Pager() = default;

    bool hasMore() {
        return auditor_->audit(__FUNCTION__, context_, hostSession_, [&] {
            return PagerInterface_->hasMore(context_, hostSession_);
        });
    }

    Page nextPage(std::size_t pageSize) {
        return auditor_->audit(
            __FUNCTION__, context_, hostSession_, pageSize,
            [&] {
                return PagerInterface_->nextPage(context_, hostSession_, pageSize);
            });
    }

 private:
    ContextConstPtr context_;
    HostSessionPtr hostSession_;
    log::AuditorPtr auditor_;
    PagerInterface<Elem>::Ptr PagerInterface_;
};
```


#### List provided to a callback

Questions:

* Should callbacks include requested page size? Or page number? In some
  kind of symmetry with other callback signatures.
* Does an error callback really make sense?

```c++
struct PageError {
    enum class ErrorCode {
        // ...
    };

    ErrorCode code;
    Str message;
};

template <class Elem>
using PageSuccessCallback = std::function<void(std::vector<Elem>)>;

using PageErrorCallback = std::function<void(PageError)>;
```

ManagerInterface:

```c++
template <class Elem>
class AbstractPager {
 public:
    // ...

    virtual void nextPage(
            const ContextConstPtr& context,
            const HostSessionPtr& hostSession,
            std::size_t pageSize
            const PageSuccessCallback& successCb,
            const PageErrorCallback& errorCb) = 0;
};
```

Manager:

```c++
template <class Elem>
class Pager {
 public:
    // ...

    void nextPage(std::size_t pageSize,
                  const PageSuccessCallback& successCb,
                  const PageErrorCallback& errorCb) {

        static constexpr  kFuncName = __FUNCTION__;

        auditor_->audit(
            kFuncName, context_, hostSession_, pageSize,
            [&] {
                abstractPager_->nextPage(
                  context_, hostSession_, pageSize
                  [&successCb, &auditor_](Page page) {
                    auditor_->audit(kFuncName, "successCb", page, [&] {
                        successCb(std::move(page));
                    })
                  },
                  [&errorCb, &auditor_](PageError error) {
                    auditor_->audit(kFuncName, "errorCb", error, [&] {
                        errorCb(std::move(error));
                    })
                  });
            });
    }

 private:
    ContextConstPtr context_;
    HostSessionPtr hostSession_;
    log::AuditorPtr auditor_;
    AbstractPager<Elem>::Ptr abstractPager_;
};
```

#### Return a single record
