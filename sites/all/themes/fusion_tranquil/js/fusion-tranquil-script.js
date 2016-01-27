// $Id: fusion-tranquil-script.js 7193 2010-04-26 16:46:46Z jeremy $

Drupal.behaviors.fusion_tranquilFirstWord = function (context) {
  $('#site-name a').each(function(){
      var bt = $(this);
      bt.html( bt.text().replace(/(^\w+)/,'<span class="first-word">$1</span>') );
  });
};

Drupal.behaviors.fusion_tranquilRoundedCorner = function (context) {
  $("#primary-menu .inner").corner("top 5px");
  $("#preface-top-inner").corner("bottom 5px"); 
   $("h2.comments-title").corner("top 5px"); 
  $(".tranquil-lightgreenbg .inner .content").corner("bottom 5px"); 
  $(".tranquil-lightgreenbg h2.title").corner("top 5px"); 
  $(".tranquil-lightgreen-title h2.title").corner("top 5px"); 
  $(".tranquil-darkteal-title h2.title").corner("top 5px"); 
  $(".tranquil-peach-title h2.title").corner("top 5px"); 
  $("div.messages").corner("5px"); 
};

Drupal.behaviors.fusion_tranquilSubmitButton = function (context) {
  // IE6 & less-specific functions
  // Add hover class to primary menu li elements on hover
  if ($.browser.msie && ($.browser.version < 7)) {
    $('span.button span input').hover(function() {
      $(this).addClass('hover');
      }, function() {
        $(this).removeClass('hover');
    }); 
    $('span.button-wrapper').hover(function() {
      $(this).addClass('hover');
      }, function() {
        $(this).removeClass('hover');
    });
  };
};