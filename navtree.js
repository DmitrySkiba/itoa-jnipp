var NAVTREE =
[
  [ "JNIpp", "index.html", [
    [ "Home", "index.html", null ],
    [ "Class List", "annotated.html", [
      [ "jni::AbstractObject", "classjni_1_1_abstract_object.html", null ],
      [ "java::CharSequence", "classjava_1_1_char_sequence.html", null ],
      [ "java::Class", "classjava_1_1_class.html", null ],
      [ "java::Exception", "classjava_1_1_exception.html", null ],
      [ "jni::LObject", "classjni_1_1_l_object.html", null ],
      [ "java::Object", "classjava_1_1_object.html", null ],
      [ "java::ObjectArray< ObjectType >", "classjava_1_1_object_array.html", null ],
      [ "java::ObjectPointer< ObjectType >", "classjava_1_1_object_pointer.html", null ],
      [ "java::PrimitiveArray< JType >", "classjava_1_1_primitive_array.html", null ],
      [ "java::RuntimeException", "classjava_1_1_runtime_exception.html", null ],
      [ "java::String", "classjava_1_1_string.html", null ],
      [ "java::Throwable", "classjava_1_1_throwable.html", null ],
      [ "jni::VarArgs", "classjni_1_1_var_args.html", null ]
    ] ],
    [ "Class Index", "classes.html", null ],
    [ "Class Hierarchy", "hierarchy.html", [
      [ "jni::AbstractObject", "classjni_1_1_abstract_object.html", [
        [ "java::Object", "classjava_1_1_object.html", [
          [ "java::CharSequence", "classjava_1_1_char_sequence.html", [
            [ "java::String", "classjava_1_1_string.html", null ]
          ] ],
          [ "java::Class", "classjava_1_1_class.html", null ],
          [ "java::ObjectArray< ObjectType >", "classjava_1_1_object_array.html", null ],
          [ "java::PrimitiveArray< JType >", "classjava_1_1_primitive_array.html", null ],
          [ "java::Throwable", "classjava_1_1_throwable.html", [
            [ "java::Exception", "classjava_1_1_exception.html", [
              [ "java::RuntimeException", "classjava_1_1_runtime_exception.html", null ]
            ] ]
          ] ]
        ] ],
        [ "java::ObjectPointer< ObjectType >", "classjava_1_1_object_pointer.html", null ],
        [ "jni::LObject", "classjni_1_1_l_object.html", null ]
      ] ],
      [ "jni::VarArgs", "classjni_1_1_var_args.html", null ]
    ] ],
    [ "Class Members", "functions.html", null ],
    [ "Namespace List", "namespaces.html", [
      [ "java", "namespacejava.html", null ],
      [ "jb", "namespacejb.html", null ],
      [ "jni", "namespacejni.html", null ]
    ] ],
    [ "Namespace Members", "namespacemembers.html", null ],
    [ "File List", "files.html", [
      [ "/Volumes/E/Github/itoa-jnipp/include/JNIpp.h", "_j_n_ipp_8h.html", null ],
      [ "/Volumes/E/Github/itoa-jnipp/include/JNIpp/JavaArray.h", "_java_array_8h.html", null ],
      [ "/Volumes/E/Github/itoa-jnipp/include/JNIpp/JavaBinding.h", "_java_binding_8h.html", null ],
      [ "/Volumes/E/Github/itoa-jnipp/include/JNIpp/JavaLang.h", "_java_lang_8h.html", null ],
      [ "/Volumes/E/Github/itoa-jnipp/include/JNIpp/JavaNI.h", "_java_n_i_8h.html", null ],
      [ "/Volumes/E/Github/itoa-jnipp/include/JNIpp/JavaObject.h", "_java_object_8h.html", null ],
      [ "/Volumes/E/Github/itoa-jnipp/include/JNIpp/JavaObjectPointer.h", "_java_object_pointer_8h.html", null ]
    ] ],
    [ "Examples", "examples.html", [
      [ "EmailValidator.cpp", "_email_validator_8cpp-example.html", null ],
      [ "EmailValidator.h", "_email_validator_8h-example.html", null ],
      [ "LiveThread.cpp", "_live_thread_8cpp-example.html", null ],
      [ "LiveThread.h", "_live_thread_8h-example.html", null ],
      [ "LiveThreadJava.java", "_live_thread_java_8java-example.html", null ],
      [ "NativeSound.cpp", "_native_sound_8cpp-example.html", null ],
      [ "NativeSound.h", "_native_sound_8h-example.html", null ],
      [ "NativeSound.java", "_native_sound_8java-example.html", null ],
      [ "NativeSoundException.java", "_native_sound_exception_8java-example.html", null ]
    ] ],
    [ "File Members", "globals.html", null ]
  ] ]
];

function createIndent(o,domNode,node,level)
{
  if (node.parentNode && node.parentNode.parentNode)
  {
    createIndent(o,domNode,node.parentNode,level+1);
  }
  var imgNode = document.createElement("img");
  if (level==0 && node.childrenData)
  {
    node.plus_img = imgNode;
    node.expandToggle = document.createElement("a");
    node.expandToggle.href = "javascript:void(0)";
    node.expandToggle.onclick = function() 
    {
      if (node.expanded) 
      {
        $(node.getChildrenUL()).slideUp("fast");
        if (node.isLast)
        {
          node.plus_img.src = node.relpath+"ftv2plastnode.png";
        }
        else
        {
          node.plus_img.src = node.relpath+"ftv2pnode.png";
        }
        node.expanded = false;
      } 
      else 
      {
        expandNode(o, node, false);
      }
    }
    node.expandToggle.appendChild(imgNode);
    domNode.appendChild(node.expandToggle);
  }
  else
  {
    domNode.appendChild(imgNode);
  }
  if (level==0)
  {
    if (node.isLast)
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2plastnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2lastnode.png";
        domNode.appendChild(imgNode);
      }
    }
    else
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2pnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2node.png";
        domNode.appendChild(imgNode);
      }
    }
  }
  else
  {
    if (node.isLast)
    {
      imgNode.src = node.relpath+"ftv2blank.png";
    }
    else
    {
      imgNode.src = node.relpath+"ftv2vertline.png";
    }
  }
  imgNode.border = "0";
}

function newNode(o, po, text, link, childrenData, lastNode)
{
  var node = new Object();
  node.children = Array();
  node.childrenData = childrenData;
  node.depth = po.depth + 1;
  node.relpath = po.relpath;
  node.isLast = lastNode;

  node.li = document.createElement("li");
  po.getChildrenUL().appendChild(node.li);
  node.parentNode = po;

  node.itemDiv = document.createElement("div");
  node.itemDiv.className = "item";

  node.labelSpan = document.createElement("span");
  node.labelSpan.className = "label";

  createIndent(o,node.itemDiv,node,0);
  node.itemDiv.appendChild(node.labelSpan);
  node.li.appendChild(node.itemDiv);

  var a = document.createElement("a");
  node.labelSpan.appendChild(a);
  node.label = document.createTextNode(text);
  a.appendChild(node.label);
  if (link) 
  {
    a.href = node.relpath+link;
  } 
  else 
  {
    if (childrenData != null) 
    {
      a.className = "nolink";
      a.href = "javascript:void(0)";
      a.onclick = node.expandToggle.onclick;
      node.expanded = false;
    }
  }

  node.childrenUL = null;
  node.getChildrenUL = function() 
  {
    if (!node.childrenUL) 
    {
      node.childrenUL = document.createElement("ul");
      node.childrenUL.className = "children_ul";
      node.childrenUL.style.display = "none";
      node.li.appendChild(node.childrenUL);
    }
    return node.childrenUL;
  };

  return node;
}

function showRoot()
{
  var headerHeight = $("#top").height();
  var footerHeight = $("#nav-path").height();
  var windowHeight = $(window).height() - headerHeight - footerHeight;
  navtree.scrollTo('#selected',0,{offset:-windowHeight/2});
}

function expandNode(o, node, imm)
{
  if (node.childrenData && !node.expanded) 
  {
    if (!node.childrenVisited) 
    {
      getNode(o, node);
    }
    if (imm)
    {
      $(node.getChildrenUL()).show();
    } 
    else 
    {
      $(node.getChildrenUL()).slideDown("fast",showRoot);
    }
    if (node.isLast)
    {
      node.plus_img.src = node.relpath+"ftv2mlastnode.png";
    }
    else
    {
      node.plus_img.src = node.relpath+"ftv2mnode.png";
    }
    node.expanded = true;
  }
}

function getNode(o, po)
{
  po.childrenVisited = true;
  var l = po.childrenData.length-1;
  for (var i in po.childrenData) 
  {
    var nodeData = po.childrenData[i];
    po.children[i] = newNode(o, po, nodeData[0], nodeData[1], nodeData[2],
        i==l);
  }
}

function findNavTreePage(url, data)
{
  var nodes = data;
  var result = null;
  for (var i in nodes) 
  {
    var d = nodes[i];
    if (d[1] == url) 
    {
      return new Array(i);
    }
    else if (d[2] != null) // array of children
    {
      result = findNavTreePage(url, d[2]);
      if (result != null) 
      {
        return (new Array(i).concat(result));
      }
    }
  }
  return null;
}

function initNavTree(toroot,relpath)
{
  var o = new Object();
  o.toroot = toroot;
  o.node = new Object();
  o.node.li = document.getElementById("nav-tree-contents");
  o.node.childrenData = NAVTREE;
  o.node.children = new Array();
  o.node.childrenUL = document.createElement("ul");
  o.node.getChildrenUL = function() { return o.node.childrenUL; };
  o.node.li.appendChild(o.node.childrenUL);
  o.node.depth = 0;
  o.node.relpath = relpath;

  getNode(o, o.node);

  o.breadcrumbs = findNavTreePage(toroot, NAVTREE);
  if (o.breadcrumbs == null)
  {
    o.breadcrumbs = findNavTreePage("index.html",NAVTREE);
  }
  if (o.breadcrumbs != null && o.breadcrumbs.length>0)
  {
    var p = o.node;
    for (var i in o.breadcrumbs) 
    {
      var j = o.breadcrumbs[i];
      p = p.children[j];
      expandNode(o,p,true);
    }
    p.itemDiv.className = p.itemDiv.className + " selected";
    p.itemDiv.id = "selected";
    $(window).load(showRoot);
  }
}

