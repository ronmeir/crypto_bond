/*!CK:1935600995!*//*1404702051,*/

if (self.CavalryLogger) { CavalryLogger.start_js(["eNPTe"]); }

__d("EntstreamAttachmentRelatedShare",["Arbiter","AsyncRequest","AttachmentRelatedShareConstants","csx","cx","CSS","DOM","Event","ge","tidyEvent","XPubcontentInlinePhotoPivotsEventsControllerURIBuilder"],function(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q){var r=2,s=3,t={createRelatedAttachmentDelay:function(u,v,w){var x=null;if(typeof u==="string"){x=o(u);}else x=u;if(!x)return;setTimeout(function(){g.inform(i.ARTICLE_CLICK,{attachment:x,global_share_id:v,is_auto_expand:true,is_right_click:false});},1000);},loadRelatedAttachment:function(u,v,w){var x=null;if(typeof u==="string"){x=o(u);}else x=u;if(!x)return;var y=n.listen(x,'click',function(){y.remove();g.inform(i.ARTICLE_CLICK,{attachment:x,global_share_id:v,is_right_click:false,share_id:w});}),z=n.listen(x,'mousedown',function(event){if(event.which===s||event.button===r){z.remove();g.inform(i.ARTICLE_CLICK,{attachment:x,global_share_id:v,is_right_click:true});}});},loadInlineStoryPivotAttachment:function(u,v){var w=o(u);if(!w)return;var x=n.listen(w,'click',function(){x.remove();g.inform(i.PHOTO_CLICK,{attachment:w,storyid:v});});},loadRelatedGameAttachment:function(u,v){var w=null;if(typeof u==="string"){w=o(u);}else w=u;if(!w)return;p(n.listen(w,'click',function(){g.inform(i.GAME_CLICK,{attachment:w,global_share_id:v});}));p(n.listen(w,'mousedown',function(event){if(event.which===s||event.button===r)g.inform(i.GAME_CLICK,{attachment:w,global_share_id:v});}));},loadRelatedLSCVideoAttachment:function(u,v){var w=null;if(typeof u==='string'){w=o(u);}else w=u;if(!w)return;var x="^div._4-u2",y=m.scry(w,x),z=n.listen(w,'click',function(){z.remove();g.inform(i.VIDEO_CLICK,{attachment:w,global_share_id:v});});},loadRelatedLSCInlineVideoAttachment:function(u,v){var w=null;if(typeof u==='string'){w=o(u);}else w=u;if(!w)return;n.listen(w,'click',function(){var x="^div._4-u2",y="_1d8v",z=m.scry(w,x),aa=z.length===1?z[0]:null,ba=aa.parentNode,ca=ba.previousSibling;while(!ca){ba=ba.parentNode;ca=ba.previousSibling;}if(!l.hasClass(ca,y)){var da=m.create('div',{className:y}),ea=m.insertBefore(aa.parentNode,da),fa=ea.length>=1?ea[0]:null;}else fa=ca;var ga=m.getID(fa);new h().setURI('/ajax/flash/expand_inline.php').setData({share_id:v,target_div:ga,max_width:470,max_height:264,replace_target_div:true,expand_you_tube_video:true}).setMethod('GET').setReadOnly(true).setRelativeTo(w.parentNode).send();g.inform(i.VIDEO_CLICK,{attachment:w,global_share_id:v,continued_chaining:true});});},closeButton:function(u,v){n.listen(u,'click',function(){m.remove(v);});},closeButtonPhotoPivots:function(u,v,w,x){n.listen(u,'click',function(){var y=new q(),z={story_id:w,search_query_type:x,event:'hide'};new h().setMethod('POST').setURI(y.getURI()).setData(z).send();m.remove(v);});},seeAllLinkPhotoPivots:function(u,v,w){n.listen(u,'click',function(){var x=new q(),y={story_id:v,search_query_type:w,event:'see_all'};new h().setMethod('POST').setURI(x.getURI()).setData(y).send();});},loadRelatedVideos:function(u,v,w){var x=o(u);if(!x)return;var y=o(v);if(!y)return;var z={global_share_id:w,attachment_div_id:m.getID(x.parentNode),video_div_id:v},aa=n.listen(y,'click',function(){aa.remove();g.inform(i.VIDEO_CLICK,{attachment:x.parentNode,attachment_div_id:m.getID(x.parentNode),video_div_id:v,global_share_id:w});});},removeOldSuggestions:function(u){var v=o(u);if(!v)return;var w=m.scry(v.parentNode,"._5d73");for(var x=1;x<w.length;x++)m.remove(w[x]);setTimeout(function(){l.show(w[0]);},1000);}};e.exports=t;},null);
__d("XPubcontentInlinePhotoPivotsHideControllerURIBuilder",["XControllerURIBuilder"],function(a,b,c,d,e,f,g){e.exports=g.create("\/pubcontent\/inline_photo_pivots_chaining\/hide\/",{});},null);