using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;
using Xamarin.Forms;
using Xamarin.Forms.Platform.iOS;
using UIKit;
using MyBooks.iOS.Renderers;

[assembly: ExportRenderer(typeof(MyBooksPage), typeof(MyBooksPageRenderer))]
namespace MyBooks.iOS
{

    public class MyBooksPageRenderer : PageRenderer
    {
        public new MyBooksPage Element
        {
            get { return (MyBooksPage)base.Element; }
        }

        public override  void ViewWillAppear(MyBooksPageRenderer instance, bool animated)
        {
            base.ViewWillAppear(animated);

            var LeftNavList = new List<UIBarButtonItem>();

            var rightNavList = new List<UIBarButtonItem>();

            var navigationItem = instance.NavigationController.TopViewController.NavigationItem;

            for (var i = 0; i < Element.ToolbarItems.Count; i++)
            {

                var reorder = (Element.ToolbarItems.Count - 1);
                var ItemPriority = Element.ToolbarItems[reorder - i].Priority;

                if (ItemPriority == 1)
                {
                    UIBarButtonItem LeftNavItems = navigationItem.RightBarButtonItems[i];
                    LeftNavList.Add(LeftNavItems);
                }
                else if (ItemPriority == 0)
                {
                    UIBarButtonItem RightNavItems = navigationItem.RightBarButtonItems[i];
                    rightNavList.Add(RightNavItems);
                }
            }

            navigationItem.SetLeftBarButtonItems(LeftNavList.ToArray(), false);
            navigationItem.SetRightBarButtonItems(rightNavList.ToArray(), false);

        }
    }
}
